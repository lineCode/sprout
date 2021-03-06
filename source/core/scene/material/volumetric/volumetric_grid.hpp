#ifndef SU_CORE_SCENE_MATERIAL_VOLUMETRIC_GRID_HPP
#define SU_CORE_SCENE_MATERIAL_VOLUMETRIC_GRID_HPP

#include "image/texture/texture_adapter.hpp"
#include "volumetric_density.hpp"
#include "volumetric_octree.hpp"

namespace scene::material::volumetric {

class Grid final : public Density {
  public:
    Grid(Sampler_settings const& sampler_settings, Texture_adapter const& grid) noexcept;

    ~Grid() noexcept override final;

    void compile() noexcept override final;

    Gridtree const* volume_tree() const noexcept override final;

    bool is_heterogeneous_volume() const noexcept override final;

    size_t num_bytes() const noexcept override final;

  private:
    float density(float3 const& uvw, Filter filter, Worker const& worker) const
        noexcept override final;

    Texture_adapter grid_;

    Gridtree tree_;
};

class Emission_grid final : public Material {
  public:
    Emission_grid(Sampler_settings const& sampler_settings, Texture_adapter const& grid) noexcept;

    ~Emission_grid() noexcept override final;

    float3 emission(math::Ray const& ray, Transformation const& transformation, float step_size,
                    rnd::Generator& rng, Filter filter, Worker const& worker) const
        noexcept override final;

    size_t num_bytes() const noexcept override final;

  private:
    float3 emission(float3 const& p, Filter filter, Worker const& worker) const noexcept;

    Texture_adapter grid_;
};

class Flow_vis_grid final : public Material {
  public:
    Flow_vis_grid(Sampler_settings const& sampler_settings, Texture_adapter const& grid) noexcept;

    ~Flow_vis_grid() noexcept override final;

    float3 emission(math::Ray const& ray, Transformation const& transformation, float step_size,
                    rnd::Generator& rng, Filter filter, Worker const& worker) const
        noexcept override final;

    size_t num_bytes() const noexcept override final;

  private:
    float density(float3 const& p, Filter filter, Worker const& worker) const noexcept;

    float3 emission(float3 const& p, Filter filter, Worker const& worker) const noexcept;

    Texture_adapter grid_;
};

}  // namespace scene::material::volumetric

#endif
