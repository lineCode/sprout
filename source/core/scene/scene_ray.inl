#ifndef SU_CORE_SCENE_RAY_INL
#define SU_CORE_SCENE_RAY_INL

#include "scene_ray.hpp"
#include "base/math/ray.inl"

namespace scene {

inline Ray::Ray(const float3& origin, const float3& direction, float min_t, float max_t,
				uint32_t depth, float time, float wavelength/*, Properties properties*/) :
	math::Ray(origin, direction, min_t, max_t, depth),
	time(time), wavelength(wavelength)/*, properties(properties)*/ {}
}

#endif
