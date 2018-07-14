#pragma once

#include "scene/material/material_sample.hpp"

namespace scene::material::glass {

class Sample_thin : public material::Sample {
  public:
    virtual Layer const& base_layer() const override final;

    virtual bxdf::Result evaluate(f_float3 wi) const override final;

    virtual void sample(sampler::Sampler& sampler, bxdf::Sample& result) const override final;

    virtual bool is_translucent() const override final;

    struct Layer : public material::Sample::Layer {
        void set(float3 const& refraction_color, float3 const& absorption_coefficient, float ior,
                 float ior_outside, float thickess);

        float3 color_;
        float3 absorption_coefficient_;
        float  ior_;
        float  ior_outside_;
        float  thickness_;
    };

    Layer layer_;

    class BSDF {
      public:
        static float reflect(const Sample_thin& sample, Layer const& layer,
                             sampler::Sampler& sampler, bxdf::Sample& result);

        static float refract(const Sample_thin& sample, Layer const& layer,
                             sampler::Sampler& sampler, bxdf::Sample& result);
    };
};

}  // namespace scene::material::glass
