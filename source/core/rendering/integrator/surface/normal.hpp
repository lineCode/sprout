#pragma once

#include "rendering/integrator/integrator.hpp"
#include "image/texture/sampler/sampler_2d_nearest.hpp"
#include "image/texture/sampler/address_mode.hpp"

namespace rendering {

class Normal : public Surface_integrator {
public:

	struct Settings {
		image::sampler::Sampler_2D_nearest<image::sampler::Address_mode_repeat> sampler;
	};

	Normal(math::random::Generator& rng, const Settings& settings);

	virtual void start_new_pixel(uint32_t num_samples);

	virtual math::float3 li(Worker& worker, uint32_t subsample, math::Oray& ray, scene::Intersection& intersection);

private:

	Settings settings_;
};

class Normal_factory : public Surface_integrator_factory {
public:

	Normal_factory();

	virtual Surface_integrator* create(math::random::Generator& rng) const;

private:

	Normal::Settings settings_;
};

}
