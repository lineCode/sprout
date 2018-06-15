#include "cloth_sample.hpp"
#include "base/math/math.hpp"
#include "base/math/sampling/sampling.hpp"
#include "base/math/vector4.inl"
#include "rendering/integrator/integrator_helper.hpp"
#include "sampler/sampler.hpp"
#include "scene/material/lambert/lambert.inl"
#include "scene/material/material_sample.inl"

namespace scene::material::cloth {

const material::Sample::Layer& Sample::base_layer() const {
    return layer_;
}

bxdf::Result Sample::evaluate(f_float3 wi, bool /*avoid_caustics*/) const {
    float const n_dot_wi = layer_.clamp_n_dot(wi);
    float const pdf      = n_dot_wi * math::Pi_inv;
    return {pdf * layer_.diffuse_color, pdf};
}

void Sample::sample(sampler::Sampler& sampler, bool /*avoid_caustics*/,
                    bxdf::Sample&     result) const {
    if (!same_hemisphere(wo_)) {
        result.pdf = 0.f;
        return;
    }

    float const n_dot_wi = lambert::Isotropic::reflect(layer_.diffuse_color, layer_, sampler,
                                                       result);
    result.reflection *= n_dot_wi;
}

void Sample::Layer::set(float3 const& color) {
    this->diffuse_color = color;
}

}  // namespace scene::material::cloth
