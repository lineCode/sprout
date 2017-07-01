#include "pathtracer.hpp"
#include "integrator_helper.hpp"
#include "sub/sub_integrator.hpp"
#include "rendering/rendering_worker.hpp"
#include "image/texture/sampler/sampler_linear_2d.inl"
#include "image/texture/sampler/sampler_nearest_2d.inl"
#include "scene/scene.hpp"
#include "scene/scene_constants.hpp"
#include "scene/scene_intersection.inl"
#include "scene/scene_ray.inl"
#include "scene/light/light.hpp"
#include "scene/light/light_sample.hpp"
#include "scene/material/bxdf.hpp"
#include "scene/material/material.hpp"
#include "scene/material/material_sample.inl"
#include "base/spectrum/rgb.hpp"
#include "base/math/vector3.inl"
#include "base/memory/align.hpp"
#include "base/random/generator.inl"

namespace rendering { namespace integrator { namespace surface {

Pathtracer::Pathtracer(rnd::Generator& rng, const take::Settings& take_settings,
					   const Settings& settings, sub::Integrator& subsurface) :
	Integrator(rng, take_settings),
	settings_(settings),
	subsurface_(subsurface),
	sampler_(rng),
	material_samplers_{rng, rng, rng},
	transmittance_(rng, take_settings) {}

Pathtracer::~Pathtracer() {
	memory::safe_destruct(subsurface_);
}

void Pathtracer::prepare(const Scene& /*scene*/, uint32_t num_samples_per_pixel) {
	sampler_.resize(num_samples_per_pixel, 1, 1, 1);

	for (auto& s : material_samplers_) {
		s.resize(num_samples_per_pixel, 1, 1, 1);
	}
}

void Pathtracer::resume_pixel(uint32_t sample, rnd::Generator& scramble) {
	sampler_.resume_pixel(sample, scramble);

	for (auto& s : material_samplers_) {
		s.resume_pixel(sample, scramble);
	}
}

float4 Pathtracer::li(Worker& worker, Ray& ray, Intersection& intersection) {
	Sampler_filter filter;
	Bxdf_result sample_result;
	Bxdf_result::Type_flag previous_sample_type;

	float3 throughput(1.f);
	float3 result(0.f);
	float opacity = 0.f;

	// pathtracer needs as many iterations as bounces, because it has no forward prediction
	for (uint32_t i = 0;; ++i) {
		bool primary_ray = 0 == i || previous_sample_type.test(Bxdf_type::Specular);

		if (primary_ray) {
			filter = Sampler_filter::Unknown;
		} else {
			filter = Sampler_filter::Nearest;
		}

		if (!resolve_mask(worker, ray, intersection, filter)) {
			break;
		}

		if (i > 0) {
			float3 tr;
			const float4 vli = worker.volume_li(ray, tr);
			result += throughput * vli.xyz();
			throughput *= tr;
		}

		opacity = 1.f;

		const float3 wo = -ray.direction;
		auto& material_sample = intersection.sample(wo, ray.time, worker, filter);

		if (material_sample.same_hemisphere(wo)) {
			result += throughput * material_sample.radiance();
		}

		if (i == settings_.max_bounces) {
			break;
		}

		if (material_sample.is_pure_emissive()) {
			break;
		}

		if (i > settings_.min_bounces) {
			if (rendering::russian_roulette(throughput, settings_.path_continuation_probability,
											sampler_.generate_sample_1D())) {
				break;
			}
		}

		material_sample.sample(material_sampler(i), sample_result);
		if (0.f == sample_result.pdf) {
			break;
		}

		if (!settings_.enable_caustics && ray.depth > 0
		&&  sample_result.type.test(Bxdf_type::Specular)) {
			break;
		}

		if (sample_result.type.test(Bxdf_type::Transmission)) {
			if (sample_result.type.test(Bxdf_type::SSS)) {
				result += throughput * subsurface_.li(worker, ray, intersection, material_sample,
													  Sampler_filter::Nearest, sample_result);
				if (0.f == sample_result.pdf) {
					break;
				}

				throughput *= sample_result.reflection / sample_result.pdf;
			} else {
				const float3 tr = transmittance_.resolve(worker, ray, intersection,
														 material_sample.absorption_coffecient(),
														 sampler_, Sampler_filter::Nearest,
														 sample_result);

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

		previous_sample_type = sample_result.type;

		const float ray_offset = take_settings_.ray_offset_factor * intersection.geo.epsilon;
		ray.origin = intersection.geo.p;
		ray.set_direction(sample_result.wi);
		ray.min_t = ray_offset;
		ray.max_t = scene::Ray_max_t;
		++ray.depth;

		if (!worker.intersect(ray, intersection)) {
			break;
		}
	}

	return float4(result, opacity);
}

sampler::Sampler& Pathtracer::material_sampler(uint32_t bounce) {
	if (Num_material_samplers > bounce) {
		return material_samplers_[bounce];
	}

	return sampler_;
}

size_t Pathtracer::num_bytes() const {
	size_t sampler_bytes = 0;

	for (auto& s : material_samplers_) {
		sampler_bytes += s.num_bytes();
	}

	return sizeof(*this) + sampler_.num_bytes() + sampler_bytes;
}

Pathtracer_factory::Pathtracer_factory(const take::Settings& take_settings,
									   uint32_t num_integrators,
									   sub::Factory* sub_factory,
									   uint32_t min_bounces, uint32_t max_bounces,
									   float path_termination_probability,
									   bool enable_caustics) :
	Factory(take_settings),
	sub_factory_(sub_factory),
	integrators_(memory::allocate_aligned<Pathtracer>(num_integrators)),
	settings_ {
		min_bounces,
		max_bounces,
		1.f - path_termination_probability,
		enable_caustics
	} {}

Pathtracer_factory::~Pathtracer_factory() {
	memory::free_aligned(integrators_);
	delete sub_factory_;
}

Integrator* Pathtracer_factory::create(uint32_t id, rnd::Generator& rng) const {
	return new(&integrators_[id]) Pathtracer(rng, take_settings_, settings_,
											 *sub_factory_->create(id, rng));
}

}}}
