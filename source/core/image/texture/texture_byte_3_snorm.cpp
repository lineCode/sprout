#include "texture_byte_3_snorm.hpp"
#include "texture_encoding.hpp"
#include "image/typed_image.inl"
#include "base/math/vector.inl"

namespace image { namespace texture {

Byte_3_snorm::Byte_3_snorm(std::shared_ptr<Image> image) :
	Texture(image),
	image_(*dynamic_cast<const Byte_3*>(image.get())) {}

float Byte_3_snorm::at_1(int32_t x, int32_t y) const {
	auto value = image_.load(x, y);
	return encoding::snorm_to_float(value.x);
}

float2 Byte_3_snorm::at_2(int32_t x, int32_t y) const {
	auto value = image_.load(x, y);
	return float2(encoding::snorm_to_float(value.x),
				  encoding::snorm_to_float(value.y));
}

float3 Byte_3_snorm::at_3(int32_t x, int32_t y) const {
	auto value = image_.load(x, y);
	return float3(encoding::snorm_to_float(value.x),
				  encoding::snorm_to_float(value.y),
				  encoding::snorm_to_float(value.z));
}

float4 Byte_3_snorm::at_4(int32_t x, int32_t y) const {
	auto value = image_.load(x, y);
	return float4(encoding::snorm_to_float(value.x),
				  encoding::snorm_to_float(value.y),
				  encoding::snorm_to_float(value.z),
				  1.f);
}

void Byte_3_snorm::gather_3(int4 xy_xy1, float3 c[4]) const {
	auto v00 = image_.load(xy_xy1.x, xy_xy1.y);
	auto v01 = image_.load(xy_xy1.x, xy_xy1.w);
	auto v10 = image_.load(xy_xy1.z, xy_xy1.y);
	auto v11 = image_.load(xy_xy1.z, xy_xy1.w);

	c[0] = float3(encoding::snorm_to_float(v00.x),
				  encoding::snorm_to_float(v00.y),
				  encoding::snorm_to_float(v00.z));

	c[1] = float3(encoding::snorm_to_float(v01.x),
				  encoding::snorm_to_float(v01.y),
				  encoding::snorm_to_float(v01.z));

	c[2] = float3(encoding::snorm_to_float(v10.x),
				  encoding::snorm_to_float(v10.y),
				  encoding::snorm_to_float(v10.z));

	c[3] = float3(encoding::snorm_to_float(v11.x),
				  encoding::snorm_to_float(v11.y),
				  encoding::snorm_to_float(v11.z));
}

float Byte_3_snorm::at_element_1(int32_t x, int32_t y, int32_t element) const {
	auto value = image_.load_element(x, y, element);
	return encoding::snorm_to_float(value.x);
}

float2 Byte_3_snorm::at_element_2(int32_t x, int32_t y, int32_t element) const {
	auto value = image_.load_element(x, y, element);
	return float2(encoding::snorm_to_float(value.x),
				  encoding::snorm_to_float(value.y));
}

float3 Byte_3_snorm::at_element_3(int32_t x, int32_t y, int32_t element) const {
	auto value = image_.load_element(x, y, element);
	return float3(encoding::snorm_to_float(value.x),
				  encoding::snorm_to_float(value.y),
				  encoding::snorm_to_float(value.z));
}

float4 Byte_3_snorm::at_element_4(int32_t x, int32_t y, int32_t element) const {
	auto value = image_.load_element(x, y, element);
	return float4(encoding::snorm_to_float(value.x),
				  encoding::snorm_to_float(value.y),
				  encoding::snorm_to_float(value.z),
				  1.f);
}

float Byte_3_snorm::at_1(int32_t x, int32_t y, int32_t z) const {
	auto value = image_.load(x, y, z);
	return encoding::snorm_to_float(value.x);
}

float2 Byte_3_snorm::at_2(int32_t x, int32_t y, int32_t z) const {
	auto value = image_.load(x, y, z);
	return float2(encoding::snorm_to_float(value.x),
				  encoding::snorm_to_float(value.y));
}

float3 Byte_3_snorm::at_3(int32_t x, int32_t y, int32_t z) const {
	auto value = image_.load(x, y, z);
	return float3(encoding::snorm_to_float(value.x),
				  encoding::snorm_to_float(value.y),
				  encoding::snorm_to_float(value.z));
}

float4 Byte_3_snorm::at_4(int32_t x, int32_t y, int32_t z) const {
	auto value = image_.load(x, y, z);
	return float4(encoding::snorm_to_float(value.x),
				  encoding::snorm_to_float(value.y),
				  encoding::snorm_to_float(value.z),
				  1.f);
}

}}
