#pragma once

#include "texture_2d.hpp"
#include "image/typed_image.hpp"

namespace image { namespace texture {

class Texture_2D_byte_2_snorm : public Texture_2D {
public:

	Texture_2D_byte_2_snorm(std::shared_ptr<Image> image);

	virtual float        at_1(uint32_t x, uint32_t y) const final override;
	virtual math::float2 at_2(uint32_t x, uint32_t y) const final override;
	virtual math::float3 at_3(uint32_t x, uint32_t y) const final override;
	virtual math::float4 at_4(uint32_t x, uint32_t y) const final override;

private:

	const Image_byte_2& image_;
};

}}