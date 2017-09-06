#pragma once

#include "ray.hpp"
#include "vector3.inl"

namespace math {

inline Ray::Ray(const float3& origin, const float3& direction, float min_t, float max_t) :
	origin(origin),
	direction(direction),
	inv_direction(reciprocal(direction)),
	min_t(min_t),
	max_t(max_t) {
	signs[0] = inv_direction[0] < 0.f ? 1u : 0u;
	signs[1] = inv_direction[1] < 0.f ? 1u : 0u;
	signs[2] = inv_direction[2] < 0.f ? 1u : 0u;
}

inline Ray::Ray(const float3& origin, const float3& direction, const float3& inv_direction,
				float min_t, float max_t, uint8_t sign_x, uint8_t sign_y, uint8_t sign_z) :
	origin(origin),
	direction(direction),
	inv_direction(inv_direction),
	min_t(min_t),
	max_t(max_t),
	signs{sign_x, sign_y, sign_z} {}

inline void Ray::set_direction(const float3& v) {
	direction = v;
	inv_direction = reciprocal(v);

	signs[0] = inv_direction[0] < 0.f ? 1u : 0u;
	signs[1] = inv_direction[1] < 0.f ? 1u : 0u;
	signs[2] = inv_direction[2] < 0.f ? 1u : 0u;
}

inline Vector3f_a Ray::point(float t) const {
	return origin + t * direction;
}

inline float Ray::length() const {
	return math::length((min_t - max_t) * direction);
}

inline Ray Ray::normalized() const {
	const float length = math::length(direction);
	return Ray(origin, direction / length, length * inv_direction,
			   min_t * length, max_t * length, signs[0], signs[1], signs[2]);
}

}
