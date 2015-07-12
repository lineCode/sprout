#pragma once

#include "image.hpp"
#include "base/color/color.hpp"

namespace image {

template<typename T>
class Typed_image : public Image {
public:

	Typed_image(const Image::Description& description);
	~Typed_image();

	const T& at(uint32_t index) const;
	void set(uint32_t index, const T& value);

	virtual const void* data() const final override;

private:

	T* data_;
};

typedef Typed_image<unsigned char>  Image_byte_1;
typedef Typed_image<color::Color2c> Image_byte_2;
typedef Typed_image<color::Color3c> Image_byte_3;
typedef Typed_image<math::float3>   Image_float_3;
typedef Typed_image<math::float4>   Image_float_4;

}
