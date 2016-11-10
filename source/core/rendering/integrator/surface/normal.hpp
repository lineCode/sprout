#pragma once

#include "rendering/integrator/surface/surface_integrator.hpp"
#include "image/texture/sampler/sampler_2d_nearest.hpp"
#include "image/texture/sampler/address_mode.hpp"

namespace rendering { namespace integrator { namespace surface {

class Normal : public Integrator {

public:

	struct Settings {
		enum class Vector {
			Tangent,
			Bitangent,
			Geometric_normal,
			Shading_normal
		};

		Vector vector;
	};

	Normal(uint32_t num_samples_per_pixel,
		   const take::Settings& take_settings,
		   random::Generator& rng,
		   const Settings& settings);

	virtual void resume_pixel(uint32_t sample, uint2 seed) final override;

	virtual float4 li(Worker& worker, scene::Ray& ray, bool volume,
					  scene::Intersection& intersection) final override;

private:

	Settings settings_;
};

class Normal_factory : public Factory {

public:

	Normal_factory(const take::Settings& take_settings,
				   Normal::Settings::Vector vector);

	virtual Integrator* create(uint32_t num_samples_per_pixel,
							   random::Generator& rng) const final override;

private:

	Normal::Settings settings_;
};

}}}
