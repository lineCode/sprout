#pragma once

#include "rendering/integrator/integrator.hpp"
#include "sampler/random_sampler.hpp"
#include "image/texture/sampler/sampler_2d_nearest.hpp"
#include "image/texture/sampler/address_mode.hpp"
#include <vector>

namespace scene {

namespace light { struct Sample; }
namespace material { class Sample; }

}

namespace rendering {

class Pathtracer_MIS : public Surface_integrator {
public:

	struct Settings {
		uint32_t min_bounces;
		uint32_t max_bounces;
		uint32_t max_light_samples;

		image::sampler::Sampler_2D_nearest<image::sampler::Address_mode_repeat> sampler;
	};

	Pathtracer_MIS(const take::Settings& take_settings, math::random::Generator& rng, const Settings& settings);

	virtual void start_new_pixel(uint32_t num_samples) final override;

	virtual math::float3 li(Worker& worker, math::Oray& ray, scene::Intersection& intersection) final override;

private:

	math::float3 estimate_direct_light(Worker& worker, const math::Oray& ray,
									   const scene::Intersection& intersection, const scene::material::Sample& material_sample);

	bool resolve_mask(Worker& worker, math::Oray& ray, scene::Intersection& intersection);

	Settings settings_;

	sampler::Random sampler_;

	std::vector<scene::light::Sample> light_samples_;
};

class Pathtracer_MIS_factory : public Surface_integrator_factory {
public:

	Pathtracer_MIS_factory(const take::Settings& take_settings, uint32_t min_bounces, uint32_t max_bounces, uint32_t max_light_samples);

	virtual Surface_integrator* create(math::random::Generator& rng) const final override;

private:

	Pathtracer_MIS::Settings settings_;
};

}



