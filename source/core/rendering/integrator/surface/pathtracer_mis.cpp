#include "pathtracer_mis.hpp"
#include "rendering/rendering_worker.hpp"
#include "rendering/integrator/integrator_helper.hpp"
#include "scene/scene.hpp"
#include "scene/scene_constants.hpp"
#include "scene/scene_ray.inl"
#include "scene/light/light.hpp"
#include "scene/light/light_sample.hpp"
#include "scene/material/bxdf.hpp"
#include "scene/material/material.hpp"
#include "scene/material/material_sample.inl"
#include "scene/prop/prop_intersection.inl"
#include "base/math/vector4.inl"
#include "base/memory/align.hpp"
#include "base/random/generator.inl"
#include "base/spectrum/rgb.hpp"

#include "base/debug/assert.hpp"

namespace rendering::integrator::surface {

Pathtracer_MIS::Pathtracer_MIS(rnd::Generator& rng, const take::Settings& take_settings,
							   const Settings& settings) :
	Integrator(rng, take_settings),
	settings_(settings),
	sampler_(rng),
	material_samplers_{rng, rng, rng},
	light_samplers_{rng, rng, rng} {}

Pathtracer_MIS::~Pathtracer_MIS() {}

void Pathtracer_MIS::prepare(const Scene& scene, uint32_t num_samples_per_pixel) {
	const uint32_t num_lights = static_cast<uint32_t>(scene.lights().size());

	num_lights_reciprocal_ = num_lights > 0 ? 1.f / static_cast<float>(num_lights) : 0.f;

	sampler_.resize(num_samples_per_pixel, 1, 1, 1);

	for (auto& s : material_samplers_) {
		s.resize(num_samples_per_pixel, 1, 1, 1);
	}

	const uint32_t num_light_samples = settings_.light_sampling.num_samples;

	if (Light_sampling::Strategy::Single == settings_.light_sampling.strategy) {
		for (auto& s : light_samplers_) {
			s.resize(num_samples_per_pixel, num_light_samples, 1, 2);
		}
	} else {
		for (auto& s : light_samplers_) {
			s.resize(num_samples_per_pixel, num_light_samples, num_lights, num_lights);
		}
	}
}

void Pathtracer_MIS::resume_pixel(uint32_t sample, rnd::Generator& scramble) {
	sampler_.resume_pixel(sample, scramble);

	for (auto& s : material_samplers_) {
		s.resume_pixel(sample, scramble);
	}

	for (auto& s : light_samplers_) {
		s.resume_pixel(sample, scramble);
	}
}

float3 Pathtracer_MIS::li(Ray& ray, Intersection& intersection, Worker& worker) {
	const uint32_t max_bounces = settings_.max_bounces;

	Sampler_filter filter = ray.is_primary() ? Sampler_filter::Undefined 
											 : Sampler_filter::Nearest;
	Bxdf_sample sample_result;

	bool treat_as_singular = true;

	float3 throughput(1.f);
	float3 result(0.f);

	for (uint32_t i = ray.depth;; ++i) {
		float3 wo = -ray.direction;
		const auto& material_sample = intersection.sample(wo, ray, filter, sampler_, worker);

		// Only check for the very first hit.
		// Subsequent hits are handled by the MIS scheme.
		if (0 == i && material_sample.same_hemisphere(wo)) {
			result += material_sample.radiance();
		}

		if (material_sample.is_pure_emissive()) {
			return result;
		}

		const float ray_offset = take_settings_.ray_offset_factor * intersection.geo.epsilon;

		result += throughput * sample_lights(ray, ray_offset, intersection,
											 material_sample, filter, worker);

		SOFT_ASSERT(math::all_finite_and_positive(result));

		const float previous_bxdf_pdf = sample_result.pdf;

		// Material BSDF importance sample
		material_sample.sample(material_sampler(ray.depth), sample_result);

		if (0.f == sample_result.pdf) {
			break;
		}

		const bool singular = sample_result.type.test_any(Bxdf_type::Specular,
														  Bxdf_type::Transmission);

		if (singular) {
			if (settings_.disable_caustics && !ray.is_primary()
			&&  material_sample.ior_greater_one()
			&&  worker.interface_stack().top_ior() == 1.f) {
				break;
			}

			if (material_sample.ior_greater_one()) {
				const bool scattering = intersection.material()->is_scattering_volume();
				treat_as_singular = scattering ? ray.is_primary() : true;
			}
		} else {
			ray.set_primary(false);
			filter = Sampler_filter::Nearest;
			treat_as_singular = false;
		}

		const bool is_translucent = material_sample.is_translucent();

		if (0.f == ray.wavelength) {
			ray.wavelength = sample_result.wavelength;
		}

		if (material_sample.ior_greater_one()) {
			ray.origin = intersection.geo.p;
			ray.set_direction(sample_result.wi);
			ray.min_t = ray_offset;
			ray.max_t = scene::Ray_max_t;
			++ray.depth;
		} else {
			ray.min_t = ray.max_t + ray_offset;
			ray.max_t = scene::Ray_max_t;
		}

		throughput *= sample_result.reflection / sample_result.pdf;

		if (sample_result.type.test(Bxdf_type::Transmission)) {
			worker.interface_change(sample_result.wi, intersection);
		}

		if (!worker.interface_stack().empty()) {
			float3 vli;
			float3 vtr;
			float3 weight;
			const bool hit = worker.volume(ray, intersection, filter, vli, vtr, weight);

			result += throughput * vli;
			vtr *= weight;
			throughput *= vtr;

			if (!hit) {
				break;
			}
		} else if (!worker.intersect_and_resolve_mask(ray, intersection, filter)) {
			break;
		}

		SOFT_ASSERT(math::all_finite_and_positive(result));

		if (!material_sample.ior_greater_one() && !treat_as_singular) {
			sample_result.pdf = previous_bxdf_pdf;
		}

		if (worker.interface_stack().top_ior() == 1.f || treat_as_singular) {
			bool pure_emissive;
			const float3 radiance = evaluate_light(ray, intersection, sample_result,
												   treat_as_singular, is_translucent, filter,
												   worker, pure_emissive);

			result += throughput * radiance;

			if (pure_emissive) {
				break;
			}
		}

		if (ray.depth >= max_bounces - 1) {
			break;
		}

		if (ray.depth > settings_.min_bounces) {
			const float q = std::max(spectrum::luminance(throughput),
									 settings_.path_continuation_probability);
			if (rendering::russian_roulette(throughput, q, sampler_.generate_sample_1D())) {
				break;
			}
		}
	}

	return result;
}

size_t Pathtracer_MIS::num_bytes() const {
	size_t sampler_bytes = 0;

	for (const auto& s : material_samplers_) {
		sampler_bytes += s.num_bytes();
	}

	for (const auto& s : light_samplers_) {
		sampler_bytes += s.num_bytes();
	}

	return sizeof(*this) + sampler_.num_bytes() + sampler_bytes;
}

float3 Pathtracer_MIS::sample_lights(const Ray& ray, float ray_offset, Intersection& intersection,
									 const Material_sample& material_sample,
									 Sampler_filter filter, Worker& worker) {
	float3 result(0.f);

	if (!material_sample.ior_greater_one()) {
		return result;
	}

	const bool do_mis = worker.interface_stack().top_ior() == 1.f;

	if (Light_sampling::Strategy::Single == settings_.light_sampling.strategy) {
		for (uint32_t i = settings_.light_sampling.num_samples; i > 0; --i) {
			const float select = light_sampler(ray.depth).generate_sample_1D(1);

			const auto light = worker.scene().random_light(select);

			result += evaluate_light(light.ref, light.pdf, ray, ray_offset, 0, do_mis,
									 intersection, material_sample, filter, worker);
		}

		result *= settings_.num_light_samples_reciprocal;
	} else {
		const auto& lights = worker.scene().lights();
		const float light_weight = num_lights_reciprocal_;
		for (uint32_t l = 0, len = static_cast<uint32_t>(lights.size()); l < len; ++l) {
			const auto& light = *lights[l];
			for (uint32_t i = settings_.light_sampling.num_samples; i > 0; --i) {
				result += evaluate_light(light, light_weight, ray, ray_offset, l, do_mis,
										 intersection, material_sample, filter, worker);
			}
		}

		result *= settings_.num_light_samples_reciprocal * light_weight;
	}

	return result;
}

float3 Pathtracer_MIS::evaluate_light(const Light& light, float light_weight, const Ray& history,
									  float ray_offset, uint32_t sampler_dimension, bool do_mis,
									  const Intersection& intersection,
									  const Material_sample& material_sample,
									  Sampler_filter filter, Worker& worker) {
	// Light source importance sample
	scene::light::Sample light_sample;
	if (!light.sample(intersection.geo.p, material_sample.geometric_normal(),
					  history.time, material_sample.is_translucent(),
					  light_sampler(history.depth),
					  sampler_dimension, Sampler_filter::Nearest, worker, light_sample)) {
		return float3(0.f);
	}

	const float shadow_offset = take_settings_.ray_offset_factor * light_sample.shape.epsilon;
	Ray shadow_ray(intersection.geo.p, light_sample.shape.wi, ray_offset,
				   light_sample.shape.t - shadow_offset, history.depth, history.time,
				   history.wavelength, history.properties);

	const float3 tv = worker.tinted_visibility(shadow_ray, intersection, filter);

	SOFT_ASSERT(math::all_finite_and_positive(tv));

	if (math::any_greater_zero(tv)) {
		const float3 tr = worker.transmittance(shadow_ray);

		SOFT_ASSERT(math::all_finite_and_positive(tr));

		const auto bxdf = material_sample.evaluate(light_sample.shape.wi);

		const float light_pdf = light_sample.shape.pdf * light_weight;
		const float weight = do_mis ? power_heuristic(light_pdf, bxdf.pdf) : 1.f;

		return (weight / light_pdf) * (tv * tr) * (light_sample.radiance * bxdf.reflection);
	}

	return float3(0.f);
}

float3 Pathtracer_MIS::evaluate_light(const Ray& ray, const Intersection& intersection,
									  Bxdf_sample sample_result, bool treat_as_singular,
									  bool is_translucent, Sampler_filter filter,
									  Worker& worker, bool& pure_emissive) {
	const uint32_t light_id = intersection.light_id();
	if (!Light::is_light(light_id)) {
		pure_emissive = false;
		return float3::identity();
	}

	float light_pdf = 0.f;

	if (!treat_as_singular) {
		auto light = worker.scene().light(light_id);

		if (Light_sampling::Strategy::All == settings_.light_sampling.strategy) {
			light.pdf = num_lights_reciprocal_;
		}

		const float ls_pdf = light.ref.pdf(ray, intersection.geo, is_translucent,
										   Sampler_filter::Nearest, worker);

		if (0.f == ls_pdf) {
			pure_emissive = true;
			return float3::identity();
		}

		light_pdf = ls_pdf * light.pdf;
	}

	const float3 wo = -sample_result.wi;

	// This will invalidate the contents of previous previous material samples.
	const auto& light_material_sample = intersection.sample(wo, ray, filter, sampler_, worker);

	pure_emissive = light_material_sample.is_pure_emissive();

	if (light_material_sample.same_hemisphere(wo)) {
		const float3 ls_energy = light_material_sample.radiance();

		const float weight = power_heuristic(sample_result.pdf, light_pdf);

		const float3 radiance = weight * ls_energy;

		SOFT_ASSERT(math::all_finite_and_positive(radiance));

		return radiance;
	}

	return float3::identity();
}

sampler::Sampler& Pathtracer_MIS::material_sampler(uint32_t bounce) {
	if (Num_material_samplers > bounce) {
		return material_samplers_[bounce];
	}

	return sampler_;
}

sampler::Sampler& Pathtracer_MIS::light_sampler(uint32_t bounce) {
	if (Num_light_samplers > bounce) {
		return light_samplers_[bounce];
	}

	return sampler_;
}

Pathtracer_MIS_factory::Pathtracer_MIS_factory(const take::Settings& take_settings,
											   uint32_t num_integrators,
											   uint32_t min_bounces, uint32_t max_bounces,
											   float path_termination_probability,
											   Light_sampling light_sampling,
											   bool enable_caustics) :
	Factory(take_settings),
	integrators_(memory::allocate_aligned<Pathtracer_MIS>(num_integrators)),
	settings_{
		min_bounces,
		max_bounces,
		1.f - path_termination_probability,
		light_sampling,
		1.f / static_cast<float>(light_sampling.num_samples),
		!enable_caustics
	} {}

Pathtracer_MIS_factory::~Pathtracer_MIS_factory() {
	memory::free_aligned(integrators_);
}

Integrator* Pathtracer_MIS_factory::create(uint32_t id, rnd::Generator& rng) const {
	return new(&integrators_[id]) Pathtracer_MIS(rng, take_settings_, settings_);
}

}
