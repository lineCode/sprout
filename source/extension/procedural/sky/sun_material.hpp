#pragma once

#include "sky_material_base.hpp"

namespace procedural::sky {

class Sun_material : public Material {
  public:
    Sun_material(Model& model);

    virtual const scene::material::Sample& sample(f_float3 wo, const scene::Renderstate& rs,
                                                  Sampler_filter filter, sampler::Sampler& sampler,
                                                  const scene::Worker& worker,
                                                  uint32_t             depth) const override final;

    virtual float3 sample_radiance(f_float3 wi, float2 uv, float area, float time,
                                   Sampler_filter       filter,
                                   const scene::Worker& worker) const override final;

    virtual float3 average_radiance(float area) const override final;

    virtual void prepare_sampling(const scene::shape::Shape& shape, uint32_t part,
                                  Transformation const& transformation, float area,
                                  bool importance_sampling, thread::Pool& pool) override final;

    virtual float ior() const override final;

    virtual size_t num_bytes() const override final;
};

}  // namespace procedural::sky
