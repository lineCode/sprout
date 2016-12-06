#pragma once

#include "base/math/vector.hpp"

namespace scene { namespace shape {

struct Intersection {
	float3 p;		// posisition in world space
	float3 t, b, n;	// interpolated tangent frame in world space
	float3 geo_n;	// geometry normal in world space
	float2 uv;		// texture coordinates

	float epsilon;
	uint32_t part;
};

}}