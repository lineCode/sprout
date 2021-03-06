#include "debug_sample.hpp"
#include "base/math/sampling.inl"
#include "sampler/sampler.hpp"
#include "scene/material/bxdf.hpp"
#include "scene/material/material_sample.inl"

namespace scene::material::debug {

static float3 constexpr color_front(0.5f, 1.f, 0.f);
static float3 constexpr color_back(1.f, 0.f, 0.5f);

const material::Layer& Sample::base_layer() const noexcept {
    return layer_;
}

bxdf::Result Sample::evaluate(float3 const& wi, bool) const noexcept {
    float3 const n = math::cross(layer_.t_, layer_.b_);

    bool const same_side = math::dot(n, layer_.n_) > 0.f;

    float const n_dot_wi = layer_.clamp_n_dot(wi);

    float3 const color = same_side ? color_front : color_back;

    float const pdf = n_dot_wi * math::Pi_inv;

    float3 const lambert = math::Pi_inv * color;

    return {n_dot_wi * lambert, pdf};
}

void Sample::sample(sampler::Sampler& sampler, bxdf::Sample& result) const noexcept {
    float3 const n = math::cross(layer_.t_, layer_.b_);

    bool const same_side = math::dot(n, layer_.n_) > 0.f;

    float2 const s2d = sampler.generate_sample_2D();

    float3 const is = math::sample_hemisphere_cosine(s2d);

    float3 const wi = math::normalize(layer_.tangent_to_world(is));

    float const n_dot_wi = layer_.clamp_n_dot(wi);

    float3 const color = same_side ? color_front : color_back;

    result.reflection = n_dot_wi * math::Pi_inv * color;
    result.wi         = wi;
    result.pdf        = n_dot_wi * math::Pi_inv;
    result.wavelength = 0.f;
    result.type.clear(bxdf::Type::Diffuse_reflection);
}

}  // namespace scene::material::debug
