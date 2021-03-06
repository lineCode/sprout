#ifndef SU_CORE_SCENE_SHAPE_TRIANGLE_PRIMITIVE_MT_HPP
#define SU_CORE_SCENE_SHAPE_TRIANGLE_PRIMITIVE_MT_HPP

#include "base/math/vector4.hpp"
#include "scene/shape/shape_vertex.hpp"

namespace math {
struct Ray;
}

namespace scene::shape::triangle {

struct Triangle_MT {
    Triangle_MT(const shape::Vertex& a, const shape::Vertex& b, const shape::Vertex& c,
                float bitangent_sign, uint32_t material_index) noexcept;

    struct Vertex {
        Vertex(const shape::Vertex& v) noexcept;

        float3 p, n, t;
        float2 uv;
    };

    Vertex   a, b, c;
    float    bitangent_sign;
    uint32_t material_index;

    bool intersect(math::Ray& ray, float2& uv) const noexcept;

    bool intersect_p(math::Ray const& ray) const noexcept;

    void interpolate(float2 uv, float3& p, float3& n, float2& tc) const noexcept;

    void interpolate(float2 uv, float3& p, float2& tc) const noexcept;

    void interpolate(float2 uv, float3& p) const noexcept;

    void interpolate_data(float2 uv, float3& n, float3& t, float2& tc) const noexcept;

    float2 interpolate_uv(float2 uv) const noexcept;

    float3 normal() const noexcept;

    float area() const noexcept;
    float area(float3 const& scale) const noexcept;
};

struct Shading_vertex_MT {
    float3   n, t;
    float2   uv;
    float    bitangent_sign;
    uint32_t material_index;
};

struct Shading_vertex_MTC {
    Shading_vertex_MTC(packed_float3 const& n, packed_float3 const& t, float2 uv) noexcept;

    float4 n_u;
    float4 t_v;
};

struct Vertex_MTC {
    Vertex_MTC(packed_float3 const& p, packed_float3 const& n, packed_float3 const& t,
               float2 uv) noexcept;

    float3 p;
    float4 n_u;
    float4 t_v;
};

}  // namespace scene::shape::triangle

#endif
