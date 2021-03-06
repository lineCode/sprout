#ifndef SU_CORE_SCENE_RENDERSTATE_HPP
#define SU_CORE_SCENE_RENDERSTATE_HPP

#include "base/math/vector3.inl"

namespace scene {

struct Renderstate {
    float3 tangent_to_world(float3 const& v) const noexcept {
        return float3(v[0] * t[0] + v[1] * b[0] + v[2] * n[0],
                      v[0] * t[1] + v[1] * b[1] + v[2] * n[1],
                      v[0] * t[2] + v[1] * b[2] + v[2] * n[2]);
    }

    float3 tangent_to_world(float2 v) const noexcept {
        return float3(v[0] * t[0] + v[1] * b[0], v[0] * t[1] + v[1] * b[1],
                      v[0] * t[2] + v[1] * b[2]);
    }

    float3 p;        // posisition in world space
    float3 t, b, n;  // interpolated tangent frame in world space
    float3 geo_n;    // geometry normal in world space
    float2 uv;       // texture coordinates

    uint64_t time;

    float area;
    float ior;
    float wavelength;
    bool  subsurface;
    bool  avoid_caustics;
};

}  // namespace scene

#endif
