#pragma once

#include "rendering/integrator/surface/surface_integrator.hpp"
#include "transmittance/transmittance_closed.hpp"
#include "sampler/sampler_ems.hpp"
#include "sampler/sampler_golden_ratio.hpp"
#include "sampler/sampler_random.hpp"

namespace scene { namespace light {

struct Sample;

}}

namespace rendering { namespace integrator { namespace surface {

class Pathtracer : public Integrator {

public:

	struct Settings {
		uint32_t min_bounces;
		uint32_t max_bounces;
		float    path_continuation_probability;

		bool disable_caustics;
	};

	Pathtracer(const take::Settings& take_settings, rnd::Generator& rng, const Settings& settings);

	virtual void prepare(const scene::Scene& scene, uint32_t num_samples_per_pixel) final override;

	virtual void resume_pixel(uint32_t sample, rnd::Generator& scramble) final override;

	virtual float4 li(Worker& worker, scene::Ray& ray, bool volume,
					  scene::Intersection& intersection) final override;

private:

	const Settings& settings_;

	sampler::Random sampler_;
	sampler::Golden_ratio material_sampler_;

	transmittance::Closed transmittance_;
};

class Pathtracer_factory : public Factory {

public:

	Pathtracer_factory(const take::Settings &take_settings,
					   uint32_t min_bounces, uint32_t max_bounces,
					   float path_termination_probability,
					   bool disable_caustics);

	virtual Integrator* create(rnd::Generator& rng) const final override;

private:

	Pathtracer::Settings settings_;
};

}}}
