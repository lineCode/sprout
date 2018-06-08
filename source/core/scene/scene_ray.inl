#ifndef SU_CORE_SCENE_RAY_INL
#define SU_CORE_SCENE_RAY_INL

#include "base/math/ray.inl"
#include "scene_ray.hpp"

namespace scene {

inline Ray::Ray(f_float3 origin, f_float3 direction, float min_t, float max_t, uint32_t depth,
                float time, float wavelength /*, Properties properties*/)
    : math::Ray(origin, direction, min_t, max_t, depth),
      time(time),
      wavelength(wavelength) /*, properties(properties)*/ {}
}  // namespace scene

#endif
