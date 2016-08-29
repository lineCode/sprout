#pragma once

#include "rendering/integrator/surface/surface_integrator.hpp"
#include "transmittance/transmittance_closed.hpp"
#include "transmittance/transmittance_open.hpp"
#include "sampler/sampler_random.hpp"

namespace scene {

namespace light { class Light; }
namespace material { class Sample; }

}

namespace rendering { namespace integrator { namespace surface {

class Pathtracer_MIS2 : public Integrator {

public:

	struct Settings {
		uint32_t min_bounces;
		uint32_t max_bounces;
		float    path_continuation_probability;

		Light_sampling light_sampling;

		float    num_light_samples_reciprocal;
		bool	 disable_caustics;
	};

	Pathtracer_MIS2(const take::Settings& take_settings,
					math::random::Generator& rng, const Settings& settings);

	virtual void start_new_pixel(uint32_t num_samples) final override;

	virtual float4 li(Worker& worker, scene::Ray& ray, bool volume,
					  scene::Intersection& intersection) final override;

private:

	float3 estimate_direct_light(Worker& worker, const scene::Ray& ray,
								 scene::Intersection& intersection,
								 const scene::material::Sample& material_sample,
								 Sampler_filter filter,
								 Bxdf_result& sample_result);

	float3 evaluate_light(const scene::light::Light* light, float light_weight,
						  Worker& worker, scene::Ray& ray,
						  scene::Intersection& intersection,
						  const scene::material::Sample& material_sample,
						  Sampler_filter filter,
						  Bxdf_result& sample_result);

	float3 resolve_transmission(Worker& worker, scene::Ray& ray,
								scene::Intersection& intersection,
								const float3& attenuation,
								Sampler_filter filter,
								Bxdf_result& sample_result);

	const Settings& settings_;

	sampler::Random sampler_;

	transmittance::Open   transmittance_open_;
	transmittance::Closed transmittance_closed_;
};

class Pathtracer_MIS2_factory : public Factory {

public:

	Pathtracer_MIS2_factory(const take::Settings& take_settings,
							uint32_t min_bounces, uint32_t max_bounces,
							float path_termination_probability,
							Light_sampling light_sampling,
							bool disable_caustics);

	virtual Integrator* create(math::random::Generator& rng) const final override;

private:

	Pathtracer_MIS2::Settings settings_;
};

}}}



