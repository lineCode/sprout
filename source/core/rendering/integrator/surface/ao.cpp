#include "ao.hpp"
#include "rendering/worker.hpp"
#include "scene/prop/prop_intersection.hpp"
#include "take/take_settings.hpp"
#include "base/math/sampling.hpp"
#include "base/math/vector.inl"
#include "base/math/ray.inl"
#include "base/math/random/generator.inl"

namespace rendering {

Ao::Ao(const take::Settings& take_settings, math::random::Generator& rng, const Settings& settings) :
	Surface_integrator(take_settings, rng), settings_(settings), sampler_(rng, settings.num_samples) {}

void Ao::start_new_pixel(uint32_t num_samples) {
	sampler_.restart(num_samples);
}

math::float3 Ao::li(Worker& worker, uint32_t subsample, math::Oray& /*ray*/, scene::Intersection& intersection) {
	sampler_.start_iteration(subsample);

	math::Oray occlusion_ray;
	occlusion_ray.origin = intersection.geo.p;
	occlusion_ray.min_t = take_settings_.ray_offset_modifier * intersection.geo.epsilon;
	occlusion_ray.max_t = settings_.radius;

	float result = 0.f;

	for (uint32_t i = 0; i < settings_.num_samples; ++i) {
		math::float2 sample = sampler_.generate_sample2d(i);
		math::float3 hs = math::sample_hemisphere_cosine(sample);
		math::float3 ws = intersection.geo.tangent_to_world(hs);

		occlusion_ray.set_direction(ws);

		if (worker.visibility(occlusion_ray)) {
			result += settings_.num_samples_reciprocal;
		}
	}

	return math::float3(result, result, result);
}

Ao_factory::Ao_factory(const take::Settings& settings, uint32_t num_samples, float radius) : Surface_integrator_factory(settings) {
	settings_.num_samples = num_samples;
	settings_.num_samples_reciprocal = 1.f / static_cast<float>(settings_.num_samples);
	settings_.radius = radius;
}

Surface_integrator* Ao_factory::create(math::random::Generator& rng) const {
	return new Ao(take_settings_, rng, settings_);
}

}
