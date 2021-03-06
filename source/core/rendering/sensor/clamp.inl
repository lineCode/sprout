#ifndef SU_CORE_RENDERING_SENSOR_CLAMP_INL
#define SU_CORE_RENDERING_SENSOR_CLAMP_INL

#include "base/math/vector3.inl"
#include "clamp.hpp"

namespace rendering::sensor::clamp {

inline float4 Identity::clamp(float4 const& color) const noexcept {
    return color;
}

inline Clamp::Clamp(float3 const& max) noexcept : max_(max) {}

inline float4 Clamp::clamp(float4 const& color) const noexcept {
    return float4(std::min(color[0], max_[0]), std::min(color[1], max_[1]),
                  std::min(color[2], max_[2]), color[3]);
}

}  // namespace rendering::sensor::clamp

#endif
