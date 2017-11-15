#ifndef SU_CORE_RENDERING_INTEGRATOR_VOLUME_INTEGRATOR_HPP
#define SU_CORE_RENDERING_INTEGRATOR_VOLUME_INTEGRATOR_HPP

#include "rendering/integrator/integrator.hpp"
#include "base/math/vector4.hpp"

namespace scene::volume { class Volume; }

namespace rendering {

class Worker;

namespace integrator::volume {

class Integrator : public integrator::Integrator {

public:

	using Volume = scene::volume::Volume;

	Integrator(rnd::Generator& rng, const take::Settings& settings);
	virtual ~Integrator();

	virtual float3 transmittance(const Ray& ray, const Volume& volume, const Worker& worker) = 0;

	virtual float3 li(const Ray& ray, bool primary_ray, const Volume& volume,
					  const Worker& worker, float3& transmittance) = 0;
};

class Factory {

public:

	Factory(const take::Settings& settings,	uint32_t num_integrators);
	virtual ~Factory();

	virtual Integrator* create(uint32_t id, rnd::Generator& rng) const = 0;

protected:

	const take::Settings& take_settings_;

	const uint32_t num_integrators_;
};

}}

#endif
