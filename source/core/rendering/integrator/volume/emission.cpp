#include "emission.hpp"
#include "base/math/aabb.inl"
#include "base/math/vector3.inl"
#include "base/memory/align.hpp"
#include "rendering/rendering_worker.hpp"
#include "scene/prop/prop_intersection.inl"
#include "scene/scene.hpp"
#include "scene/scene_ray.inl"
#include "scene/shape/shape.hpp"
#include "tracking.hpp"

namespace rendering::integrator::volume {

Emission::Emission(rnd::Generator& rng, take::Settings const& take_settings,
                   Settings const& settings)
    : Integrator(rng, take_settings), settings_(settings) {}

void Emission::prepare(scene::Scene const& /*scene*/, uint32_t /*num_samples_per_pixel*/) {}

void Emission::resume_pixel(uint32_t /*sample*/, rnd::Generator& /*scramble*/) {}

bool Emission::transmittance(Ray const& ray, Worker& worker, float3& transmittance) {
    return Tracking::transmittance(ray, rng_, worker, transmittance);
}

bool Emission::integrate(Ray& /*ray*/, Intersection& /*intersection*/, Sampler_filter /*filter*/,
                         Worker& /*worker*/, float3& /*li*/, float3& /*transmittance*/) {
    return false;
}

size_t Emission::num_bytes() const {
    return sizeof(*this);
}

Emission_factory::Emission_factory(take::Settings const& settings, uint32_t num_integrators,
                                   float step_size)
    : Factory(settings, num_integrators),
      integrators_(memory::allocate_aligned<Emission>(num_integrators)),
      settings_{step_size} {}

Emission_factory::~Emission_factory() {
    memory::free_aligned(integrators_);
}

Integrator* Emission_factory::create(uint32_t id, rnd::Generator& rng) const {
    return new (&integrators_[id]) Emission(rng, take_settings_, settings_);
}

}  // namespace rendering::integrator::volume
