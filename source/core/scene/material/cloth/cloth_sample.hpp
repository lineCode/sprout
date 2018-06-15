#pragma once

#include "scene/material/material_sample.hpp"

namespace scene::material::cloth {

class Sample : public material::Sample {
  public:
    virtual Layer const& base_layer() const override final;

    virtual bxdf::Result evaluate(f_float3 wi, bool avoid_caustics) const override final;

    virtual void sample(sampler::Sampler& sampler, bool avoid_caustics,
                        bxdf::Sample& result) const override final;

    struct Layer : public material::Sample::Layer {
        void set(float3 const& color);

        float3 diffuse_color;
    };

    Layer layer_;
};

}  // namespace scene::material::cloth
