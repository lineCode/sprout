#pragma once

#include "scene/material/material.hpp"

namespace scene::material::glass {

class Glass_thin : public Material {
  public:
    Glass_thin(Sampler_settings const& sampler_settings) noexcept;

    material::Sample const& sample(float3 const& wo, Renderstate const& rs, Filter filter,
                                   sampler::Sampler& sampler, Worker const& worker,
                                   uint32_t depth) const noexcept override final;

    float3 thin_absorption(float3 const& wo, float3 const& n, float2 uv, uint64_t time,
                           Filter filter, Worker const& worker) const noexcept override final;

    float ior() const noexcept override final;

    bool has_tinted_shadow() const noexcept override final;

    bool is_scattering_volume() const noexcept final override;

    size_t num_bytes() const noexcept override final;

    void set_normal_map(Texture_adapter const& normal_map) noexcept;

    void set_refraction_color(float3 const& color) noexcept;
    void set_attenuation(float3 const& absorption_color, float distance) noexcept;
    void set_ior(float ior) noexcept;
    void set_thickness(float thickness) noexcept;

    static size_t sample_size() noexcept;

  private:
    Texture_adapter normal_map_;

    float3 refraction_color_;
    float3 absorption_coefficient_;
    float  ior_;
    float  thickness_;
};

}  // namespace scene::material::glass
