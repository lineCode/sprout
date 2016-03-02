#include "texture_2d_byte_3_snorm.hpp"
#include "texture_encoding.hpp"
#include "image/typed_image.inl"
#include "base/math/vector.inl"

namespace image { namespace texture {

Texture_2D_byte_3_snorm::Texture_2D_byte_3_snorm(std::shared_ptr<Image> image) :
	Texture_2D(image),
	image_(*dynamic_cast<const Image_byte_3*>(image.get())) {}

float Texture_2D_byte_3_snorm::at_1(int32_t x, int32_t y) const {
	auto value = image_.load(x, y);
	return encoding::snorm_to_float(value.x);
}

math::float2 Texture_2D_byte_3_snorm::at_2(int32_t x, int32_t y) const {
	auto value = image_.load(x, y);
	return math::float2(encoding::snorm_to_float(value.x),
						encoding::snorm_to_float(value.y));
}

math::float3 Texture_2D_byte_3_snorm::at_3(int32_t x, int32_t y) const {
	auto value = image_.load(x, y);
	return math::float3(encoding::snorm_to_float(value.x),
						encoding::snorm_to_float(value.y),
						encoding::snorm_to_float(value.z));
}

math::float4 Texture_2D_byte_3_snorm::at_4(int32_t x, int32_t y) const {
	auto value = image_.load(x, y);
	return math::float4(encoding::snorm_to_float(value.x),
						encoding::snorm_to_float(value.y),
						encoding::snorm_to_float(value.z), 1.f);
}

float Texture_2D_byte_3_snorm::at_1(int32_t x, int32_t y, int32_t element) const {
	auto& value = image_.at(x, y, element);
	return encoding::snorm_to_float(value.x);
}

math::float2 Texture_2D_byte_3_snorm::at_2(int32_t x, int32_t y, int32_t element) const {
	auto& value = image_.at(x, y, element);
	return math::float2(encoding::snorm_to_float(value.x),
						encoding::snorm_to_float(value.y));
}

math::float3 Texture_2D_byte_3_snorm::at_3(int32_t x, int32_t y, int32_t element) const {
	auto& value = image_.at(x, y, element);
	return math::float3(encoding::snorm_to_float(value.x),
						encoding::snorm_to_float(value.y),
						encoding::snorm_to_float(value.z));
}

math::float4 Texture_2D_byte_3_snorm::at_4(int32_t x, int32_t y, int32_t element) const {
	auto& value = image_.at(x, y, element);
	return math::float4(encoding::snorm_to_float(value.x),
						encoding::snorm_to_float(value.y),
						encoding::snorm_to_float(value.z), 1.f);
}

}}
