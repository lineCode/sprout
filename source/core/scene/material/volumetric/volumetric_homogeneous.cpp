#include "volumetric_homogeneous.hpp"
#include "base/math/ray.inl"
#include "scene/material/material_sample_helper.hpp"

namespace scene::material::volumetric {

Homogeneous::Homogeneous(Sampler_settings const& sampler_settings) noexcept
    : Material(sampler_settings) {}

float3 Homogeneous::emission(math::Ray const& /*ray*/, Transformation const& /*transformation*/,
                             float /*step_size*/, rnd::Generator& /*rng*/, Filter /*filter*/,
                             Worker const& /*worker*/) const noexcept {
    return float3::identity();
}

float3 Homogeneous::absorption_coefficient(float2 /*uv*/, Filter /*filter*/,
                                           Worker const& /*worker*/) const noexcept {
    return cc_.a;
}

CC Homogeneous::collision_coefficients() const noexcept {
    return cc_;
}

CC Homogeneous::collision_coefficients(float2 /*uv*/, Filter /*filter*/,
                                       Worker const& /*worker*/) const noexcept {
    return cc_;
}

CC Homogeneous::collision_coefficients(float3 const& /*p*/, Filter /*filter*/,
                                       Worker const& /*worker*/) const noexcept {
    return cc_;
}

size_t Homogeneous::num_bytes() const noexcept {
    return sizeof(*this);
}

}  // namespace scene::material::volumetric
