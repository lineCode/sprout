#pragma once

#include "scene/material/material_sample.hpp"

namespace scene::material::display {

class Sample : public material::Sample {
  public:
    virtual Layer const& base_layer() const override final;

    virtual bxdf::Result evaluate(f_float3 wi, bool avoid_caustics) const override final;

    virtual float3 radiance() const override final;

    virtual void sample(sampler::Sampler& sampler, bool avoid_caustics,
                        bxdf::Sample& result) const override final;

    struct Layer : material::Sample::Layer {
        void set(float3 const& radiance, float f0, float roughness);

        float3 emission_;
        float3 f0_;
        float  alpha_;
        float  alpha2_;
    };

    Layer layer_;
};

}  // namespace scene::material::display
