#pragma once

#include "base/math/vector3.hpp"

namespace scene {

struct Renderstate {
	float3 tangent_to_world(float3_p v) const;

	float3 tangent_to_world(float2 v) const;

	float3 p;		// posisition in world space
	float3 t, b, n;	// interpolated tangent frame in world space
	float3 geo_n;	// geometry normal in world space
	float2 uv;		// texture coordinates

	float area;
	float time;
	float ior;
};

}
