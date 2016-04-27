#pragma once

#include "math.hpp"
#include "vector.inl"

namespace math {

inline math::float3 disk_to_hemisphere_equidistant(math::float2 uv) {
	float longitude = std::atan2(-uv.y, uv.x) ;

	float r = std::sqrt(uv.x * uv.x + uv.y * uv.y);

	// Equidistant projection
	float colatitude = r * math::Pi_div_2;

	// Equal-area projection
	// float colatitude = /*2.f **/ std::asin(r);

	// Stereographic projection
	// float colatitude = 2.f * std::atan(r);

	// spherical to cartesian
	float sin_col = std::sin(colatitude);
	float cos_col = std::cos(colatitude);
	float sin_lon = std::sin(longitude);
	float cos_lon = std::cos(longitude);

	return math::float3(sin_col * cos_lon, sin_col * sin_lon, cos_col);
}

inline math::float2 hemisphere_to_disk_equidistant(math::pfloat3 dir) {
	// cartesian to spherical
	float colatidude = std::acos(dir.z);

	float longitude = std::atan2(-dir.y, dir.x);

	float r = colatidude * (math::Pi_inv * 2.f);

	float sin_lon = std::sin(longitude);
	float cos_lon = std::cos(longitude);

	return math::float2(r * cos_lon, r * sin_lon);
}

inline math::float2 hemisphere_to_disk_paraboloid(math::pfloat3 dir) {
	float zoi = 1.f / (dir.z + 1.f);
	return math::float2(dir.x * zoi, dir.y * -zoi);
}

}