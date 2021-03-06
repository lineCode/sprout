#include "texture_float_3.hpp"
#include "base/math/vector4.inl"
#include "image/typed_image.inl"

namespace image::texture {

Float3::Float3(std::shared_ptr<Image> const& image) noexcept
    : Texture(image), image_(*static_cast<const image::Float3*>(image.get())) {}

float Float3::at_1(int32_t i) const noexcept {
    return image_.load(i)[0];
}

float3 Float3::at_3(int32_t i) const noexcept {
    return float3(image_.load(i));
}

float Float3::at_1(int32_t x, int32_t y) const noexcept {
    return image_.load(x, y)[0];
}

float2 Float3::at_2(int32_t x, int32_t y) const noexcept {
    return image_.load(x, y).xy();
}

float3 Float3::at_3(int32_t x, int32_t y) const noexcept {
    return float3(image_.load(x, y));

    //	return float3(image_.at(x, y));
}

void Float3::gather_1(int4 const& xy_xy1, float c[4]) const noexcept {
    packed_float3 v[4];
    image_.gather(xy_xy1, v);

    c[0] = v[0][0];
    c[1] = v[1][0];
    c[2] = v[2][0];
    c[3] = v[3][0];
}

void Float3::gather_2(int4 const& xy_xy1, float2 c[4]) const noexcept {
    packed_float3 v[4];
    image_.gather(xy_xy1, v);

    c[0] = v[0].xy();
    c[1] = v[1].xy();
    c[2] = v[2].xy();
    c[3] = v[3].xy();
}

void Float3::gather_3(int4 const& xy_xy1, float3 c[4]) const noexcept {
    packed_float3 v[4];
    image_.gather(xy_xy1, v);

    c[0] = float3(v[0]);
    c[1] = float3(v[1]);
    c[2] = float3(v[2]);
    c[3] = float3(v[3]);
}

float Float3::at_element_1(int32_t x, int32_t y, int32_t element) const noexcept {
    return image_.at_element(x, y, element)[0];
}

float2 Float3::at_element_2(int32_t x, int32_t y, int32_t element) const noexcept {
    return image_.at_element(x, y, element).xy();
}

float3 Float3::at_element_3(int32_t x, int32_t y, int32_t element) const noexcept {
    //	return image_.at(x, y, element);

    return float3(image_.at_element(x, y, element));
}

float Float3::at_1(int32_t x, int32_t y, int32_t z) const noexcept {
    return image_.load(x, y, z)[0];
}

float2 Float3::at_2(int32_t x, int32_t y, int32_t z) const noexcept {
    return image_.load(x, y, z).xy();
}

float3 Float3::at_3(int32_t x, int32_t y, int32_t z) const noexcept {
    return float3(image_.load(x, y, z));

    //	return float3(image_.at(x, y, z));
}

}  // namespace image::texture
