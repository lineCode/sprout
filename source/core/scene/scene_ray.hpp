#pragma once

#include "base/math/ray.hpp"

namespace scene {

struct Ray : public math::Oray {
	Ray();
	Ray(const math::float3& origin, const math::float3& direction,
		float min_t = 0.f, float max_t = 1.f,
		float time = 0.f, float ticke_time = 0.f,
		uint32_t depth = 0, float ior = 1.f);

	float	 time;
	float    tick_time;
	uint32_t depth;
	float    ior;
};

}
