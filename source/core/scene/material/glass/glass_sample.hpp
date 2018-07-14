#ifndef SU_CORE_SCENE_MATERIAL_GLASS_SAMPLE_HPP
#define SU_CORE_SCENE_MATERIAL_GLASS_SAMPLE_HPP

#include "scene/material/material_sample.hpp"

namespace scene::material::glass {

class Sample : public material::Sample {
  public:
    virtual Layer const& base_layer() const override final;

    virtual bxdf::Result evaluate(f_float3 wi) const override final;

    virtual void sample(sampler::Sampler& sampler, bxdf::Sample& result) const override;

    void set(float3 const& refraction_color, float ior, float ior_outside);

    Layer layer_;

    float3 color_;
    float  ior_;
    float  ior_outside_;

    class BSDF {
      public:
        static float reflect(f_float3 wo, f_float3 n, float n_dot_wo, bxdf::Sample& result);

        static float refract(f_float3 wo, f_float3 n, f_float3 color, float n_dot_wo, float n_dot_t,
                             float eta_i, bxdf::Sample& result);
    };
};

}  // namespace scene::material::glass

#endif
