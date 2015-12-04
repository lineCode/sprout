#include "encoding_srgb.hpp"
#include "image/typed_image.inl"
#include "base/color/color.inl"
#include "base/math/vector.inl"

namespace image { namespace encoding {

Srgb::Srgb(math::uint2 dimensions) :rgba_(new color::Color4c[dimensions.x * dimensions.y]) {}

Srgb::~Srgb() {
	delete [] rgba_;
}

void Srgb::to_sRGB(const image::Image_float_4& image, uint32_t begin, uint32_t end) {
	for (uint32_t i = begin; i < end; ++i) {
		math::float4 color = image.at(i);
		color = color::linear_to_sRGB(color);
		rgba_[i] = color::to_byte(color);
	}
}

}}