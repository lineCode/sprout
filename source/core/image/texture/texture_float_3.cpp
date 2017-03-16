#include "texture_float_3.hpp"
#include "image/typed_image.inl"
#include "base/math/vector4.inl"

namespace image { namespace texture {

Float_3::Float_3(std::shared_ptr<Image> image) :
	Texture(image),
	image_(*static_cast<const image::Float_3*>(image.get())) {}

float3 Float_3::at_3(int32_t i) const {
	return float3(image_.load(i));
}

float Float_3::at_1(int32_t x, int32_t y) const {
	return image_.load(x, y)[0];
}

float2 Float_3::at_2(int32_t x, int32_t y) const {
	return image_.load(x, y).xy();
}

float3 Float_3::at_3(int32_t x, int32_t y) const {
	return float3(image_.load(x, y));

//	return float3(image_.at(x, y));
}

float4 Float_3::at_4(int32_t x, int32_t y) const {
	return float4(image_.load(x, y), 1.f);
}

float4 Float_3::gather_1(int4 xy_xy1) const {
	packed_float3 v[4];
	image_.gather(xy_xy1, v);

	return float4(v[0][0], v[1][0], v[2][0], v[3][0]);
}

void Float_3::gather_2(int4 xy_xy1, float2 c[4]) const {
	packed_float3 v[4];
	image_.gather(xy_xy1, v);

	c[0] = v[0].xy();
	c[1] = v[1].xy();
	c[2] = v[2].xy();
	c[3] = v[3].xy();
}

void Float_3::gather_3(int4 xy_xy1, float3 c[4]) const {
/*	packed_float3 v[4];
	image_.gather(xy_xy1, v);

	c[0] = float3(v[0]);
	c[1] = float3(v[1]);
	c[2] = float3(v[2]);
	c[3] = float3(v[3]);*/

	const int32_t width = image_.description().dimensions[0];

	const int32_t y0 = width * xy_xy1[1];

	const packed_float3 v0 = image_.load(y0 + xy_xy1[0]);
	c[0] = float3(v0);

	const packed_float3 v1 = image_.load(y0 + xy_xy1[2]);
	c[1] = float3(v1);

	const int32_t y1 = width * xy_xy1[3];

	const packed_float3 v2 = image_.load(y1 + xy_xy1[0]);
	c[2] = float3(v2);

	const packed_float3 v3 = image_.load(y1 + xy_xy1[2]);
	c[3] = float3(v3);
}

float Float_3::at_element_1(int32_t x, int32_t y, int32_t element) const {
	return image_.at_element(x, y, element)[0];
}

float2 Float_3::at_element_2(int32_t x, int32_t y, int32_t element) const {
	return image_.at_element(x, y, element).xy();
}

float3 Float_3::at_element_3(int32_t x, int32_t y, int32_t element) const {
//	return image_.at(x, y, element);

	return float3(image_.at_element(x, y, element));
}

float4 Float_3::at_element_4(int32_t x, int32_t y, int32_t element) const {
	return float4(image_.at_element(x, y, element), 1.f);
}

float Float_3::at_1(int32_t x, int32_t y, int32_t z) const {
	return image_.load(x, y, z)[0];
}

float2 Float_3::at_2(int32_t x, int32_t y, int32_t z) const {
	return image_.load(x, y, z).xy();
}

float3 Float_3::at_3(int32_t x, int32_t y, int32_t z) const {
	return float3(image_.load(x, y, z));

//	return float3(image_.at(x, y, z));
}

float4 Float_3::at_4(int32_t x, int32_t y, int32_t z) const {
	return float4(image_.load(x, y, z), 1.f);
}

}}
