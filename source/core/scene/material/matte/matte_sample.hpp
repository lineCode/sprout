#pragma once

#include "scene/material/material_sample.hpp"

namespace scene::material::matte {

class Sample : public material::Sample {
  public:
    Layer const& base_layer() const noexcept override final;

    bxdf::Result evaluate(float3 const& wi, bool include_back) const noexcept override final;

    void sample(sampler::Sampler& sampler, bxdf::Sample& result) const noexcept override final;

    void set(float3 const& color) noexcept;

    Layer layer_;

  private:
    float3 diffuse_color_;

    float alpha_;
};

}  // namespace scene::material::matte
