#pragma once

#include "sampler_2d_nearest.hpp"
#include "image/texture/texture.hpp"
#include <algorithm>

namespace image { namespace texture { namespace sampler {

template<typename Address_mode>
float Sampler_2D_nearest<Address_mode>::sample_1(const Texture& texture, float2 uv) const {
	auto b = texture.back_2();
	auto d = texture.dimensions_float2();

	uv = Address_mode::f(uv);

	int32_t x = std::min(static_cast<int32_t>(uv.x * d.x), b.x);
	int32_t y = std::min(static_cast<int32_t>(uv.y * d.y), b.y);

	return texture.at_1(x, y);
}

template<typename Address_mode>
float2 Sampler_2D_nearest<Address_mode>::sample_2(const Texture& texture, float2 uv) const {
	auto b = texture.back_2();
	auto d = texture.dimensions_float2();

	uv = Address_mode::f(uv);

	int32_t x = std::min(static_cast<int32_t>(uv.x * d.x), b.x);
	int32_t y = std::min(static_cast<int32_t>(uv.y * d.y), b.y);

	return texture.at_2(x, y);
}

template<typename Address_mode>
float3 Sampler_2D_nearest<Address_mode>::sample_3(const Texture& texture, float2 uv) const {
	auto b = texture.back_2();
	auto d = texture.dimensions_float2();

	uv = Address_mode::f(uv);

	int32_t x = std::min(static_cast<int32_t>(uv.x * d.x), b.x);
	int32_t y = std::min(static_cast<int32_t>(uv.y * d.y), b.y);

	return texture.at_3(x, y);
}

template<typename Address_mode>
float Sampler_2D_nearest<Address_mode>::sample_1(const Texture& texture, float2 uv,
												 int32_t element) const {
	auto b = texture.back_2();
	auto d = texture.dimensions_float2();

	uv = Address_mode::f(uv);

	int32_t x = std::min(static_cast<int32_t>(uv.x * d.x), b.x);
	int32_t y = std::min(static_cast<int32_t>(uv.y * d.y), b.y);

	int32_t min_element = std::min(texture.num_elements() - 1, element);

	return texture.at_element_1(x, y, min_element);
}

template<typename Address_mode>
float2 Sampler_2D_nearest<Address_mode>::sample_2(const Texture& texture, float2 uv,
												  int32_t element) const {
	auto b = texture.back_2();
	auto d = texture.dimensions_float2();

	uv = Address_mode::f(uv);

	int32_t x = std::min(static_cast<int32_t>(uv.x * d.x), b.x);
	int32_t y = std::min(static_cast<int32_t>(uv.y * d.y), b.y);

	int32_t min_element = std::min(texture.num_elements() - 1, element);

	return texture.at_element_2(x, y, min_element);
}

template<typename Address_mode>
float3 Sampler_2D_nearest<Address_mode>::sample_3(const Texture& texture, float2 uv,
												  int32_t element) const {
	auto b = texture.back_2();
	auto d = texture.dimensions_float2();

	uv = Address_mode::f(uv);

	int32_t x = std::min(static_cast<int32_t>(uv.x * d.x), b.x);
	int32_t y = std::min(static_cast<int32_t>(uv.y * d.y), b.y);

	int32_t min_element = std::min(texture.num_elements() - 1, element);

	return texture.at_element_3(x, y, min_element);
}

template<typename Address_mode>
float2 Sampler_2D_nearest<Address_mode>::address(float2 uv) const {
	return Address_mode::f(uv);
}

}}}
