#include "pathtracer_mis.hpp"
#include "integrator_helper.hpp"
#include "sub/sub_integrator.hpp"
#include "rendering/rendering_worker.hpp"
#include "scene/scene.hpp"
#include "scene/scene_constants.hpp"
#include "scene/scene_ray.inl"
#include "scene/light/light.hpp"
#include "scene/light/light_sample.hpp"
#include "scene/material/bxdf.hpp"
#include "scene/material/material.hpp"
#include "scene/material/material_sample.inl"
#include "scene/scene_intersection.inl"
#include "base/math/vector4.inl"
#include "base/memory/align.hpp"
#include "base/random/generator.inl"
#include "base/spectrum/rgb.hpp"

namespace rendering { namespace integrator { namespace surface {

Pathtracer_MIS::Pathtracer_MIS(rnd::Generator& rng, const take::Settings& take_settings,
							   const Settings& settings, sub::Integrator& subsurface) :
	Integrator(rng, take_settings),
	settings_(settings),
	subsurface_(subsurface),
	sampler_(rng),
	material_samplers_{rng, rng, rng},
	light_samplers_{rng, rng, rng},
	transmittance_open_(rng, take_settings, settings.max_bounces),
	transmittance_closed_(rng, take_settings) {}

Pathtracer_MIS::~Pathtracer_MIS() {
	memory::safe_destruct(subsurface_);
}

void Pathtracer_MIS::prepare(const Scene& scene, uint32_t num_samples_per_pixel) {
	num_lights_reciprocal_ = 1.f / static_cast<float>(scene.lights().size());

	sampler_.resize(num_samples_per_pixel, 1, 1, 1);

	for (auto& s : material_samplers_) {
		s.resize(num_samples_per_pixel, 1, 1, 1);
	}

	const uint32_t num_light_samples = settings_.light_sampling.num_samples;

	if (Light_sampling::Strategy::One == settings_.light_sampling.strategy) {
		for (auto& s : light_samplers_) {
			s.resize(num_samples_per_pixel, num_light_samples, 1, 2);
		}
	} else {
		const uint32_t num_lights = static_cast<uint32_t>(scene.lights().size());
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

float4 Pathtracer_MIS::li(Worker& worker, Ray& ray, Intersection& intersection) {
	Sampler_filter filter = Sampler_filter::Unknown;
	Bxdf_result sample_result;

	float3 throughput(1.f);
	float3 result(0.f);
	float opacity = 0.f;
	bool primary_ray = 0 == ray.depth;
	bool requires_bounce = false;

	if (!resolve_mask(worker, ray, intersection, filter)) {
		return float4(result, opacity);
	}

	for (uint32_t i = 0; ; ++i) {
		if (i > 0) {
			float3 tr;
			const float4 vli = worker.volume_li(ray, tr);
			result += throughput * vli.xyz();
			throughput *= tr;
		}

		const float3 wo = -ray.direction;
		const auto& material_sample = intersection.sample(wo, ray.time, worker, filter);

		if ((primary_ray || requires_bounce) && material_sample.same_hemisphere(wo)) {
			result += throughput * material_sample.radiance();

			if (requires_bounce ? i == settings_.max_bounces + 1 : i == settings_.max_bounces) {
				break;
			}
		}

		if (material_sample.is_pure_emissive()) {
			opacity = 1.f;
			break;
		}

		result += throughput * estimate_direct_light(worker, ray, intersection, material_sample,
													 filter, sample_result, requires_bounce);

		if (!intersection.hit()
		||  (requires_bounce ? i == settings_.max_bounces : i >= settings_.max_bounces - 1)
		||  ((0.f == sample_result.pdf) | (float3::identity() == sample_result.reflection))) {
			break;
		}

		if (i > settings_.min_bounces) {
			if (rendering::russian_roulette(throughput, settings_.path_continuation_probability,
											sampler_.generate_sample_1D())) {
				break;
			}
		}

		if (sample_result.type.test(Bxdf_type::Specular)) {
			if (!settings_.enable_caustics && !primary_ray) {
				break;
			}
		} else {
			primary_ray = false;
			filter = Sampler_filter::Nearest;
		}

		if (sample_result.type.test(Bxdf_type::Transmission)) {
			if (sample_result.type.test(Bxdf_type::SSS)) {
				result += throughput * subsurface_.li(worker, ray, intersection, material_sample,
													  Sampler_filter::Nearest, sample_result);
				if (0.f == sample_result.pdf) {
					// result += float3(1.f, 0.f, 0.f);
					break;
				}

				throughput *= sample_result.reflection / sample_result.pdf;
			} else {
				const float3 tr = resolve_transmission(worker, ray, intersection,
													   material_sample.absorption_coffecient(),
													   Sampler_filter::Nearest, sample_result);
				if (0.f == sample_result.pdf) {
					break;
				}

				throughput *= tr;
				opacity += spectrum::luminance(tr);
			}
		} else {
			throughput *= sample_result.reflection / sample_result.pdf;
			opacity = 1.f;
		}

		const float ray_offset = take_settings_.ray_offset_factor * intersection.geo.epsilon;
		ray.origin = intersection.geo.p;
		ray.set_direction(sample_result.wi);
		ray.min_t = ray_offset;
		ray.max_t = scene::Ray_max_t;
		++ray.depth;

		// For these cases we fall back to plain pathtracing
		if (requires_bounce) {
			if (!intersect_and_resolve_mask(worker, ray, intersection, filter)) {
				break;
			}
		}
	}

	return float4(result, opacity);
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

float3 Pathtracer_MIS::estimate_direct_light(Worker& worker, const Ray& ray,
											 Intersection& intersection,
											 const Material_sample& material_sample,
											 Sampler_filter filter,
											 Bxdf_result& sample_result,
											 bool& requires_bounce) {
	float3 result(0.f);

	const float ray_offset = take_settings_.ray_offset_factor * intersection.geo.epsilon;

	if (Light_sampling::Strategy::One == settings_.light_sampling.strategy) {
		for (uint32_t i = settings_.light_sampling.num_samples; i > 0; --i) {
			const float select = light_sampler(ray.depth).generate_sample_1D(1);

			float light_pdf;
			const auto light = worker.scene().random_light(select, light_pdf);
			if (!light) {
				continue;
			}

			const float light_pdf_reciprocal = 1.f / light_pdf;

			result += evaluate_light(light, light_pdf_reciprocal, ray.time, ray_offset, ray.depth,
									 0, worker, intersection, material_sample, filter);
		}

		result *= settings_.num_light_samples_reciprocal;
	} else {
		const auto& lights = worker.scene().lights();
		const float light_weight = static_cast<float>(lights.size());
		for (uint32_t l = 0, len = static_cast<uint32_t>(lights.size()); l < len; ++l) {
			const auto light = lights[l];
			for (uint32_t i = settings_.light_sampling.num_samples; i > 0; --i) {
				result += evaluate_light(light, light_weight, ray.time, ray_offset, ray.depth,
										 l, worker, intersection, material_sample, filter);
			}
		}

		result *= settings_.num_light_samples_reciprocal / light_weight;
	}

	// Material BSDF importance sample
	material_sample.sample(material_sampler(ray.depth), sample_result);

	// Those cases are handled outside
	requires_bounce = sample_result.type.test_any(Bxdf_type::Specular,
												  Bxdf_type::Transmission,
												  Bxdf_type::SSS);

	if (requires_bounce || 0.f == sample_result.pdf) {
		return result;
	}

	Ray secondary_ray(intersection.geo.p, sample_result.wi,
					  ray_offset, scene::Ray_max_t, ray.time, ray.depth + 1);

	if (intersect_and_resolve_mask(worker, secondary_ray, intersection, filter)) {
		float light_pdf = 0.f;
		const auto light = worker.scene().light(intersection.light_id(), light_pdf);
		if (!light) {
			return result;
		}

		if (Light_sampling::Strategy::All == settings_.light_sampling.strategy) {
			light_pdf = num_lights_reciprocal_;
		}

		const float ls_pdf = light->pdf(secondary_ray.origin, sample_result.wi, ray.time,
										material_sample.is_translucent(),
										worker, Sampler_filter::Nearest);
		if (0.f == ls_pdf) {
			return result;
		}

		const float3 wo = -sample_result.wi;
		// This will invalidate the contents of material_sample,
		// so we must not use it after this point (e.g. in the calling function)!
		// Exceptions are the Specular and Transmission cases, which never come here.
		const auto& light_material_sample = intersection.sample(wo, ray.time, worker,
																Sampler_filter::Nearest);

		if (light_material_sample.same_hemisphere(wo)) {
			const float3 t = worker.transmittance(secondary_ray);

			const float3 ls_energy = t * light_material_sample.radiance();

			const float weight = power_heuristic(sample_result.pdf, ls_pdf * light_pdf);

			result += (weight / sample_result.pdf)
				   * (ls_energy * sample_result.reflection);
		}
	}

	return result;
}

float3 Pathtracer_MIS::evaluate_light(const scene::light::Light* light, float light_weight,
									  float time, float ray_offset, uint32_t depth,
									  uint32_t sampler_dimension, Worker& worker,
									  const Intersection& intersection,
									  const Material_sample& material_sample,
									  Sampler_filter filter) {
	// Light source importance sample
	scene::light::Sample light_sample;
	light->sample(intersection.geo.p, material_sample.geometric_normal(), time,
				  material_sample.is_translucent(), light_sampler(depth),
				  sampler_dimension, worker, Sampler_filter::Nearest, light_sample);

	if (light_sample.shape.pdf <= 0.f) {
		return float3(0.f);
	}

	const Ray shadow_ray(intersection.geo.p, light_sample.shape.wi,
						 ray_offset, light_sample.shape.t - ray_offset, time, depth);

	const float3 tv = worker.tinted_visibility(shadow_ray, filter);
	if (math::any_greater_zero(tv)) {
		const float3 t = worker.transmittance(shadow_ray);

		float bxdf_pdf;
		const float3 f = material_sample.evaluate(light_sample.shape.wi, bxdf_pdf);

		const float weight = power_heuristic(light_sample.shape.pdf / light_weight, bxdf_pdf);

		return (weight / light_sample.shape.pdf * light_weight)
			 * (tv * t) * (light_sample.radiance * f);
	}

	return float3(0.f);
}

float3 Pathtracer_MIS::resolve_transmission(Worker& worker, const Ray& ray,
											Intersection& intersection, const float3& attenuation,
											Sampler_filter filter, Bxdf_result& sample_result) {
	if (intersection.prop->is_open()) {
		return transmittance_open_.resolve(worker, ray, intersection, attenuation,
										   sampler_, filter, sample_result);
	} else {
		return transmittance_closed_.resolve(worker, ray, intersection, attenuation,
											 sampler_, filter, sample_result);
	}
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
											   sub::Factory* sub_factory,
											   uint32_t min_bounces, uint32_t max_bounces,
											   float path_termination_probability,
											   Light_sampling light_sampling,
											   bool enable_caustics) :
	Factory(take_settings),
	sub_factory_(sub_factory),
	integrators_(memory::allocate_aligned<Pathtracer_MIS>(num_integrators)),
	settings_{
		min_bounces,
		max_bounces,
		1.f - path_termination_probability,
		light_sampling,
		1.f / static_cast<float>(light_sampling.num_samples),
		enable_caustics
	} {}

Pathtracer_MIS_factory::~Pathtracer_MIS_factory() {
	memory::free_aligned(integrators_);
	delete sub_factory_;
}

Integrator* Pathtracer_MIS_factory::create(uint32_t id, rnd::Generator& rng) const {
	return new(&integrators_[id]) Pathtracer_MIS(rng, take_settings_, settings_,
												 *sub_factory_->create(id, rng));
}

}}}
