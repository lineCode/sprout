#ifndef SU_CORE_SCENE_SHAPE_SHAPE_HPP
#define SU_CORE_SCENE_SHAPE_SHAPE_HPP

#include "base/math/aabb.hpp"
#include "base/math/matrix.hpp"
#include "scene/material/material.hpp"

namespace math {
struct Transformation;
}

namespace sampler {
class Sampler;
}

namespace scene {

struct Ray;
class Worker;

namespace entity {
struct Composed_transformation;
}

namespace shape {

struct Intersection;
struct Sample_to;
struct Sample_from;
class Node_stack;
class Morphable_shape;

class Shape {
  public:
    using Transformation = entity::Composed_transformation;
    using Filter         = material::Sampler_settings::Filter;

    virtual ~Shape() noexcept;

    math::AABB const& aabb() const noexcept;

    float3 object_to_texture_point(float3 const& p) const noexcept;
    float3 object_to_texture_vector(float3 const& v) const noexcept;

    virtual math::AABB transformed_aabb(float4x4 const& m, math::Transformation const& t) const
        noexcept;
    virtual math::AABB transformed_aabb(math::Transformation const& t) const noexcept;

    virtual uint32_t num_parts() const noexcept;

    virtual bool intersect(Ray& ray, Transformation const& transformation, Node_stack& node_stack,
                           Intersection& intersection) const noexcept = 0;

    virtual bool intersect_fast(Ray& ray, Transformation const& transformation,
                                Node_stack& node_stack, Intersection& intersection) const
        noexcept = 0;

    virtual bool intersect(Ray& ray, Transformation const& transformation, Node_stack& node_stack,
                           float& epsilon) const noexcept = 0;

    virtual bool intersect_p(Ray const& ray, Transformation const& transformation,
                             Node_stack& node_stack) const noexcept = 0;

    //	virtual bool intersect_p(FVector ray_origin, FVector ray_direction,
    //							 FVector ray_min_t, FVector ray_max_t,
    //							 Transformation const& transformation,
    //							 Node_stack& node_stack) const;

    virtual float opacity(Ray const& ray, Transformation const& transformation,
                          Materials const& materials, Filter filter, Worker const& worker) const
        noexcept = 0;

    virtual float3 thin_absorption(Ray const& ray, Transformation const& transformation,
                                   Materials const& materials, Filter filter,
                                   Worker const& worker) const noexcept = 0;

    virtual bool sample(uint32_t part, float3 const& p, float3 const& n,
                        Transformation const& transformation, float area, bool two_sided,
                        sampler::Sampler& sampler, uint32_t sampler_dimension,
                        Node_stack& node_stack, Sample_to& sample) const noexcept = 0;

    virtual bool sample(uint32_t part, float3 const& p, Transformation const& transformation,
                        float area, bool two_sided, sampler::Sampler& sampler,
                        uint32_t sampler_dimension, Node_stack& node_stack, Sample_to& sample) const
        noexcept = 0;

    virtual bool sample(uint32_t part, Transformation const& transformation, float area,
                        bool two_sided, sampler::Sampler& sampler, uint32_t sampler_dimension,
                        math::AABB const& bounds, Node_stack& node_stack, Sample_from& sample) const
        noexcept = 0;

    // Both pdf functions implicitely assume that the passed
    // ray/intersection/transformation combination actually lead to a hit.
    virtual float pdf(Ray const& ray, Intersection const& intersection,
                      Transformation const& transformation, float area, bool two_sided,
                      bool total_sphere) const noexcept = 0;

    // The following three functions are used for textured lights
    // and should have the uv weight baked in!
    virtual bool sample(uint32_t part, float3 const& p, float2 uv,
                        Transformation const& transformation, float area, bool two_sided,
                        Sample_to& sample) const noexcept = 0;

    virtual bool sample(uint32_t part, float2 uv, Transformation const& transformation, float area,
                        bool two_sided, sampler::Sampler& sampler, uint32_t sampler_dimension,
                        math::AABB const& bounds, Sample_from& sample) const noexcept = 0;

    virtual float pdf_uv(Ray const& ray, Intersection const& intersection,
                         Transformation const& transformation, float area, bool two_sided) const
        noexcept = 0;

    virtual float uv_weight(float2 uv) const noexcept = 0;

    virtual float area(uint32_t part, float3 const& scale) const noexcept = 0;

    virtual bool is_complex() const noexcept;
    virtual bool is_finite() const noexcept;
    virtual bool is_analytical() const noexcept;

    virtual void prepare_sampling(uint32_t part) noexcept;

    virtual Morphable_shape* morphable_shape() noexcept;

    virtual size_t num_bytes() const noexcept = 0;

  protected:
    math::AABB aabb_;

    float3 inv_extent_;
};

}  // namespace shape
}  // namespace scene

#endif
