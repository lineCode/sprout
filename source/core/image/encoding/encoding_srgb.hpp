#pragma once

#include "image/typed_image.hpp"
#include "base/color/color.hpp"
#include "base/math/vector.hpp"

namespace image { namespace encoding {

class Srgb {
public:

	Srgb(math::uint2 dimensions);
	virtual ~Srgb();

protected:

	void to_sRGB(const image::Image_float_4& image, uint32_t begin, uint32_t end);

	color::Color4c* rgba_;
};

}}