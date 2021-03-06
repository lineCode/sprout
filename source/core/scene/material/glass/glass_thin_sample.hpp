#pragma once

#include "scene/material/material_sample.hpp"

namespace scene::material::glass {

class Sample_thin : public material::Sample {
  public:
    Layer const& base_layer() const noexcept override final;

    bxdf::Result evaluate(float3 const& wi, bool include_back) const noexcept override final;

    void sample(sampler::Sampler& sampler, bxdf::Sample& result) const noexcept override final;

    bool is_translucent() const noexcept override final;

    void set(float3 const& refraction_color, float3 const& absorption_coefficient, float ior,
             float ior_outside, float thickess) noexcept;

    Layer layer_;

    float3 color_;
    float3 absorption_coefficient_;

    float ior_;
    float ior_outside_;
    float thickness_;

    float reflect(sampler::Sampler& sampler, bxdf::Sample& result) const noexcept;

    float refract(sampler::Sampler& sampler, bxdf::Sample& result) const noexcept;
};

}  // namespace scene::material::glass
