#pragma once

#include "rendering/integrator/surface/surface_integrator.hpp"
#include "transmittance/transmittance_closed.hpp"
#include "sampler/random_sampler.hpp"
#include "image/texture/sampler/sampler_2d_linear.hpp"
#include "image/texture/sampler/sampler_2d_nearest.hpp"
#include "image/texture/sampler/address_mode.hpp"
#include <vector>

namespace scene { namespace light {

struct Sample;

}}

namespace rendering { namespace integrator { namespace surface {

class Pathtracer : public Integrator {
public:

	struct Settings {
		image::texture::sampler::Sampler_2D_linear <image::texture::sampler::Address_mode_repeat> sampler_linear;
		image::texture::sampler::Sampler_2D_nearest<image::texture::sampler::Address_mode_repeat> sampler_nearest;

		uint32_t min_bounces;
		uint32_t max_bounces;
		bool disable_caustics;
	};

	Pathtracer(const take::Settings& take_settings, math::random::Generator& rng, const Settings& settings);

	virtual void start_new_pixel(uint32_t num_samples) final override;

	virtual math::float4 li(Worker& worker, scene::Ray& ray, bool volume,
							scene::Intersection& intersection) final override;

private:

	const Settings& settings_;

	sampler::Random sampler_;

	std::vector<scene::light::Sample> light_samples_;

	transmittance::Closed transmittance_;
};

class Pathtracer_factory : public Integrator_factory {
public:

	Pathtracer_factory(const take::Settings &take_settings,
					   uint32_t min_bounces, uint32_t max_bounces,
					   bool disable_caustics);

	virtual Integrator* create(math::random::Generator& rng) const final override;

private:

	Pathtracer::Settings settings_;
};

}}}
