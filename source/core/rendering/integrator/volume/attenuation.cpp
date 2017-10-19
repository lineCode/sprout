#include "attenuation.hpp"
#include "rendering/rendering_worker.hpp"
#include "scene/scene.hpp"
#include "scene/scene_ray.inl"
#include "scene/volume/volume.hpp"
#include "base/math/aabb.inl"
#include "base/math/vector3.inl"
#include "base/memory/align.hpp"

namespace rendering { namespace integrator { namespace volume {

Attenuation::Attenuation(rnd::Generator& rng, const take::Settings& take_settings) :
	Integrator(rng, take_settings) {}

void Attenuation::prepare(const scene::Scene& /*scene*/, uint32_t /*num_samples_per_pixel*/) {}

void Attenuation::resume_pixel(uint32_t /*sample*/, rnd::Generator& /*scramble*/) {}

float3 Attenuation::transmittance(const Ray& ray, const Volume& volume, Worker& worker) {
	float min_t;
	float max_t;
	if (!worker.scene().aabb().intersect_p(ray, min_t, max_t)) {
		return float3(1.f);
	}

	scene::Ray tray(ray.origin, ray.direction, min_t, max_t, ray.time);

	float3 tau = volume.optical_depth(tray, 1.f, rng_, Sampler_filter::Nearest, worker);
	return math::exp(-tau);
}

float4 Attenuation::li(const Ray& ray, bool /*primary_ray*/, const Volume& volume,
					   Worker& worker, float3& transmittance) {
	float min_t;
	float max_t;
	if (!worker.scene().aabb().intersect_p(ray, min_t, max_t)) {
		transmittance = float3(1.f);
		return float4::identity();
	}

	const scene::Ray tray(ray.origin, ray.direction, min_t, max_t, ray.time);

	const float3 tau = volume.optical_depth(tray, 1.f, rng_, Sampler_filter::Undefined, worker);
	transmittance = math::exp(-tau);

	return float4(0.f);
}

size_t Attenuation::num_bytes() const {
	return sizeof(*this);
}

Attenuation_factory::Attenuation_factory(const take::Settings& settings, uint32_t num_integrators) :
	Factory(settings, num_integrators),
	integrators_(memory::allocate_aligned<Attenuation>(num_integrators)) {}

Attenuation_factory::~Attenuation_factory() {
	memory::free_aligned(integrators_);
}

Integrator* Attenuation_factory::create(uint32_t id, rnd::Generator& rng) const {
	return new(&integrators_[id]) Attenuation(rng, take_settings_);
}

}}}
