#pragma once

#include "sampler_linear_3d.hpp"
#include "address_mode.hpp"
#include "bilinear.hpp"
#include "image/texture/texture.hpp"
#include <algorithm>

namespace image { namespace texture { namespace sampler {

template<typename Address_mode>
float Linear_3D<Address_mode>::sample_1(const Texture& texture, float3_p uvw) const {
	int3 xyz, xyz1;
	float3 stu = map(texture, uvw, xyz, xyz1);

	float c000 = texture.at_1(xyz.x,  xyz.y,  xyz.z);
	float c010 = texture.at_1(xyz.x,  xyz1.y, xyz.z);
	float c100 = texture.at_1(xyz1.x, xyz.y,  xyz.z);
	float c110 = texture.at_1(xyz1.x, xyz1.y, xyz.z);
	float c001 = texture.at_1(xyz.x,  xyz.y,  xyz1.z);
	float c011 = texture.at_1(xyz.x,  xyz1.y, xyz1.z);
	float c101 = texture.at_1(xyz1.x, xyz.y,  xyz1.z);
	float c111 = texture.at_1(xyz1.x, xyz1.y, xyz1.z);

	float c0 = bilinear(c000, c010, c100, c110, stu.v[0], stu.v[1]);
	float c1 = bilinear(c001, c011, c101, c111, stu.v[0], stu.v[1]);

	return math::lerp(c0, c1, stu.v[2]);
}

template<typename Address_mode>
float2 Linear_3D<Address_mode>::sample_2(const Texture& texture, float3_p uvw) const {
	int3 xyz, xyz1;
	float3 stu = map(texture, uvw, xyz, xyz1);

	float2 c000 = texture.at_2(xyz.x,  xyz.y,  xyz.z);
	float2 c010 = texture.at_2(xyz.x,  xyz1.y, xyz.z);
	float2 c100 = texture.at_2(xyz1.x, xyz.y,  xyz.z);
	float2 c110 = texture.at_2(xyz1.x, xyz1.y, xyz.z);
	float2 c001 = texture.at_2(xyz.x,  xyz.y,  xyz1.z);
	float2 c011 = texture.at_2(xyz.x,  xyz1.y, xyz1.z);
	float2 c101 = texture.at_2(xyz1.x, xyz.y,  xyz1.z);
	float2 c111 = texture.at_2(xyz1.x, xyz1.y, xyz1.z);

	float2 c0 = bilinear(c000, c010, c100, c110, stu.v[0], stu.v[1]);
	float2 c1 = bilinear(c001, c011, c101, c111, stu.v[0], stu.v[1]);

	return math::lerp(c0, c1, stu.v[2]);
}

template<typename Address_mode>
float3 Linear_3D<Address_mode>::sample_3(const Texture& texture, float3_p uvw) const {
	int3 xyz, xyz1;
	float3 stu = map(texture, uvw, xyz, xyz1);

	float3 c000 = texture.at_3(xyz.x,  xyz.y,  xyz.z);
	float3 c010 = texture.at_3(xyz.x,  xyz1.y, xyz.z);
	float3 c100 = texture.at_3(xyz1.x, xyz.y,  xyz.z);
	float3 c110 = texture.at_3(xyz1.x, xyz1.y, xyz.z);
	float3 c001 = texture.at_3(xyz.x,  xyz.y,  xyz1.z);
	float3 c011 = texture.at_3(xyz.x,  xyz1.y, xyz1.z);
	float3 c101 = texture.at_3(xyz1.x, xyz.y,  xyz1.z);
	float3 c111 = texture.at_3(xyz1.x, xyz1.y, xyz1.z);

	float3 c0 = bilinear(c000, c010, c100, c110, stu.v[0], stu.v[1]);
	float3 c1 = bilinear(c001, c011, c101, c111, stu.v[0], stu.v[1]);

	return math::lerp(c0, c1, stu.v[2]);
}

template<typename Address_mode>
float3 Linear_3D<Address_mode>::address(float3_p uvw) const {
	return float3(Address_mode::f(uvw.v[0]),
				  Address_mode::f(uvw.v[1]),
				  Address_mode::f(uvw.v[2]));
}

template<typename Address_mode>
float3 Linear_3D<Address_mode>::map(const Texture& texture, float3_p uvw,
									int3& xyz, int3& xyz1) {
	auto b = texture.back_3();
	const auto d = texture.dimensions_float3();

	float u = Address_mode::f(uvw.v[0]) * d.v[0] - 0.5f;
	float v = Address_mode::f(uvw.v[1]) * d.v[1] - 0.5f;
	float w = Address_mode::f(uvw.v[2]) * d.v[2] - 0.5f;

	float fu = std::floor(u);
	float fv = std::floor(v);
	float fw = std::floor(w);

	int32_t x = static_cast<int32_t>(fu);
	int32_t y = static_cast<int32_t>(fv);
	int32_t z = static_cast<int32_t>(fw);

	xyz.x = Address_mode::lower_bound(x, b.x);
	xyz.y = Address_mode::lower_bound(y, b.y);
	xyz.z = Address_mode::lower_bound(z, b.z);

	xyz1.x = Address_mode::increment(x, b.x);
	xyz1.y = Address_mode::increment(y, b.y);
	xyz1.z = Address_mode::increment(z, b.z);

	return float3(u - fu, v - fv, w - fw);
}

}}}
