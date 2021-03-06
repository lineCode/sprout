#include "volumetric_density.hpp"
#include "base/math/matrix4x4.inl"
#include "base/math/ray.inl"
#include "base/random/generator.inl"
#include "scene/entity/composed_transformation.hpp"
#include "scene/material/material_sample_helper.hpp"

namespace scene::material::volumetric {

Density::Density(Sampler_settings const& sampler_settings) noexcept : Material(sampler_settings) {}

float3 Density::emission(math::Ray const& /*ray*/, Transformation const& /*transformation*/,
                         float /*step_size*/, rnd::Generator& /*rng*/, Filter /*filter*/,
                         Worker const& /*worker*/) const noexcept {
    return float3::identity();
}

CC Density::collision_coefficients() const noexcept {
    return cc_;
}

CC Density::collision_coefficients(float2 /*uv*/, Filter /*filter*/, Worker const& /*worker*/) const
    noexcept {
    return cc_;
}

CC Density::collision_coefficients(float3 const& uvw, Filter filter, Worker const& worker) const
    noexcept {
    float const d = density(uvw, filter, worker);

    return {d * cc_.a, d * cc_.s};
}

}  // namespace scene::material::volumetric
