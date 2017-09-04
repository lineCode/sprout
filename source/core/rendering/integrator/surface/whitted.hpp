#pragma once

#include "surface_integrator.hpp"
#include "sampler/sampler_golden_ratio.hpp"
#include "sampler/sampler_random.hpp"

namespace scene { namespace material { class Sample; } }

namespace rendering { namespace integrator { namespace surface {

class alignas(64) Whitted : public Integrator {

public:

	struct Settings {
		uint32_t num_light_samples;
		float    num_light_samples_reciprocal;
	};

	Whitted(rnd::Generator& rng, const take::Settings& take_settings, const Settings& settings);

	virtual void prepare(const Scene& scene, uint32_t num_samples_per_pixel) override final;

	virtual void resume_pixel(uint32_t sample, rnd::Generator& scramble) override final;

	virtual float4 li(Ray& ray, Intersection& intersection, Worker& worker) override final;

	virtual size_t num_bytes() const override final;

private:

	float3 shade(const Ray& ray, const Intersection& intersection, Worker& worker);

	float3 estimate_direct_light(const Ray& ray, const Intersection& intersection,
								 const Material_sample& material_sample, Worker& worker);

	const Settings settings_;

	sampler::Random sampler_;
};

class Whitted_factory : public Factory {

public:

	Whitted_factory(const take::Settings& take_settings, uint32_t num_integrators,
					uint32_t num_light_samples);

	~Whitted_factory();

	virtual Integrator* create(uint32_t id, rnd::Generator& rng) const;

private:

	Whitted* integrators_;

	Whitted::Settings settings_;
};

}}}
