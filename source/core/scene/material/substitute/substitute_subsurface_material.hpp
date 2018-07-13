#ifndef SU_CORE_SCENE_MATERIAL_SUBSTITUTE_SUBSURFACE_MATERIAL_HPP
#define SU_CORE_SCENE_MATERIAL_SUBSTITUTE_SUBSURFACE_MATERIAL_HPP

#include "scene/material/volumetric/volumetric_octree.hpp"
#include "substitute_base_material.hpp"
#include "substitute_subsurface_sample.hpp"

namespace scene::material::substitute {

class Material_subsurface final : public Material_base {
  public:
    Material_subsurface(Sampler_settings const& sampler_settings);

    virtual void compile() override final;

    virtual material::Sample const& sample(f_float3 wo, Renderstate const& rs,
                                           Sampler_filter filter, sampler::Sampler& sampler,
                                           Worker const& worker,
                                           uint32_t      depth) const override final;

    virtual size_t num_bytes() const override final;

    void set_density_map(Texture_adapter const& density_map);

    void set_attenuation(f_float3 absorption_color, f_float3 scattering_color, float distance);

    void set_volumetric_anisotropy(float anisotropy);

    virtual void set_ior(float ior, float external_ior = 1.f) final override;

    virtual float3 emission(math::Ray const& ray, Transformation const& transformation,
                            float step_size, rnd::Generator& rng, Sampler_filter filter,
                            Worker const& worker) const override final;

    virtual float3 absorption_coefficient(float2 uv, Sampler_filter filter,
                                          Worker const& worker) const override final;

    virtual CC collision_coefficients() const override final;

    virtual CC collision_coefficients(float2 uv, Sampler_filter filter,
                                      Worker const& worker) const override final;

    virtual CC collision_coefficients(f_float3 p, Sampler_filter filter,
                                      Worker const& worker) const override final;

    virtual CM control_medium() const override final;

    virtual volumetric::Gridtree const* volume_tree() const override final;

    virtual bool is_heterogeneous_volume() const override final;
    virtual bool is_textured_volume() const override final;

    static size_t sample_size();

  private:
    float density(f_float3 p, Sampler_filter filter, Worker const& worker) const;

    float3 color(f_float3 p, Sampler_filter filter, Worker const& worker) const;

    Texture_adapter density_map_;

    float3 absorption_color_;
    CC     cc_;
    CM     cm_;
    float  anisotropy_;
    float  attenuation_distance_;

    volumetric::Gridtree tree_;

    Sample_subsurface::IOR sior_;
};

}  // namespace scene::material::substitute

#endif
