#pragma once

#include "rendering/integrator/volume/volume_integrator.hpp"

namespace rendering { namespace integrator { namespace volume {

class alignas(64) Attenuation : public Integrator {

public:

	Attenuation(rnd::Generator& rng, const take::Settings& take_settings);

	virtual void prepare(const scene::Scene& scene, uint32_t num_samples_per_pixel) final override;

	virtual void resume_pixel(uint32_t sample, rnd::Generator& scramble) final override;

	virtual float3 transmittance(Worker& worker, const Ray& ray,
								 const Volume& volume) final override;

	virtual float4 li(Worker& worker, const Ray& ray, const Volume& volume,
					  float3& transmittance) final override;

	virtual size_t num_bytes() const final override;
};

class Attenuation_factory : public Factory {

public:

	Attenuation_factory(const take::Settings& settings, uint32_t num_integrators);

	~Attenuation_factory();

	virtual Integrator* create(uint32_t id, rnd::Generator& rng) const final override;

private:

	Attenuation* integrators_;
};

}}}
