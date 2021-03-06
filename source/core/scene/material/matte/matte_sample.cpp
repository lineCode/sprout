#include "matte_sample.hpp"
#include "scene/material/disney/disney.inl"
#include "scene/material/material_sample.inl"
// #include "scene/material/lambert/lambert.inl"
// #include "scene/material/oren_nayar/oren_nayar.inl"
#include "base/math/vector4.inl"

namespace scene::material::matte {

const material::Layer& Sample::base_layer() const noexcept {
    return layer_;
}

bxdf::Result Sample::evaluate(float3 const& wi, bool) const noexcept {
    float const n_dot_wi = layer_.clamp_n_dot(wi);
    float const n_dot_wo = layer_.clamp_abs_n_dot(wo_);  // layer_.clamp_n_dot(wo_);

    //	float3 brdf = lambert::Isotropic::reflection(layer_.diffuse_color, n_dot_wi, layer_, pdf);

    float3 const h = math::normalize(wo_ + wi);

    float const h_dot_wi = clamp_dot(h, wi);

    //	float const wi_dot_wo = math::dot(wi, wo_);
    //	float const sl_wi_wo = 2.f + 2.f * wi_dot_wo;
    //	float const rcpl_wi_wo = math::rsqrt(sl_wi_wo);
    //	float const h_dot_wi = math::clamp(rcpl_wi_wo + rcpl_wi_wo * wi_dot_wo, 0.00001f, 1.f);

    auto const brdf = disney::Isotropic::reflection(h_dot_wi, n_dot_wi, n_dot_wo, alpha_,
                                                    diffuse_color_);

    //	float3 brdf = oren_nayar::Isotropic::reflection(wi, wo_, n_dot_wi, n_dot_wo, layer_, pdf);

    return {n_dot_wi * brdf.reflection, brdf.pdf};
}

void Sample::sample(sampler::Sampler& sampler, bxdf::Sample& result) const noexcept {
    //	float const n_dot_wi = lambert::Isotropic::reflect(layer_.diffuse_color, layer_, sampler,
    // result);

    float const n_dot_wo = layer_.clamp_abs_n_dot(wo_);  // layer_.clamp_n_dot(wo_);

    float const n_dot_wi = disney::Isotropic::reflect(wo_, n_dot_wo, layer_, alpha_, diffuse_color_,
                                                      sampler, result);

    result.reflection *= n_dot_wi;

    result.wavelength = 0.f;
}

void Sample::set(float3 const& color) noexcept {
    diffuse_color_ = color;
    alpha_         = math::pow2(1.f);
}

}  // namespace scene::material::matte
