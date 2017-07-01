#include "ao.hpp"
#include "rendering/rendering_worker.hpp"
#include "scene/scene_intersection.inl"
#include "scene/scene_ray.inl"
#include "scene/material/material.hpp"
#include "scene/material/material_sample.inl"
#include "base/math/sampling/sampling.hpp"
#include "base/math/vector4.inl"
#include "base/memory/align.hpp"
#include "base/random/generator.inl"

namespace rendering { namespace integrator { namespace surface {

AO::AO(rnd::Generator& rng, const take::Settings& take_settings, const Settings& settings) :
	Integrator(rng, take_settings),
	settings_(settings),
	sampler_(rng) {}

void AO::prepare(const Scene& /*scene*/, uint32_t num_samples_per_pixel) {
	sampler_.resize(num_samples_per_pixel, settings_.num_samples, 1, 1);
}

void AO::resume_pixel(uint32_t sample, rnd::Generator& scramble) {
	sampler_.resume_pixel(sample, scramble);
}

float4 AO::li(Worker& worker, Ray& ray, Intersection& intersection) {
	float result = 0.f;

	if (!resolve_mask(worker, ray, intersection, Sampler_filter::Unknown)) {
		return float4(result, result, result, 1.f);
	}

	Ray occlusion_ray;
	occlusion_ray.origin = intersection.geo.p;
	occlusion_ray.min_t	 = take_settings_.ray_offset_factor * intersection.geo.epsilon;
	occlusion_ray.max_t	 = settings_.radius;
	occlusion_ray.time   = ray.time;

	const float3 wo = -ray.direction;
	const auto& material_sample = intersection.sample(wo, ray.time, worker,
													  Sampler_filter::Unknown);

	for (uint32_t i = settings_.num_samples; i > 0; --i) {
		const float2 sample = sampler_.generate_sample_2D();
		const float3 hs = math::sample_hemisphere_cosine(sample);
//		float3 ws = intersection.geo.tangent_to_world(hs);
		const float3 ws = material_sample.base_layer().tangent_to_world(hs);

		occlusion_ray.set_direction(ws);

		if (worker.masked_visibility(occlusion_ray, Sampler_filter::Unknown)) {
			result += settings_.num_samples_reciprocal;
		}
	}

	return float4(result, result, result, 1.f);
}

size_t AO::num_bytes() const {
	return sizeof(*this) + sampler_.num_bytes();
}

AO_factory::AO_factory(const take::Settings& settings, uint32_t num_integrators,
					   uint32_t num_samples, float radius) :
	Factory(settings),
	integrators_(memory::allocate_aligned<AO>(num_integrators)) {
	settings_.num_samples = num_samples;
	settings_.num_samples_reciprocal = 1.f / static_cast<float>(settings_.num_samples);
	settings_.radius = radius;
}

AO_factory::~AO_factory() {
	memory::free_aligned(integrators_);
}

Integrator* AO_factory::create(uint32_t id, rnd::Generator& rng) const {
	return new(&integrators_[id]) AO(rng, take_settings_, settings_);
}

}}}
