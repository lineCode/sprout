#pragma once

#include "scene_renderstate.hpp"
#include "base/math/vector.inl"

namespace scene {

inline float3 Renderstate::tangent_to_world(float3_p v) const{
	return float3(v.x * t.x + v.y * b.x + v.z * n.x,
				  v.x * t.y + v.y * b.y + v.z * n.y,
				  v.x * t.z + v.y * b.z + v.z * n.z);
}

inline float3 Renderstate::tangent_to_world(float2 v) const{
	return float3(v.x * t.x + v.y * b.x,
				  v.x * t.y + v.y * b.y,
				  v.x * t.z + v.y * b.z);
}

}