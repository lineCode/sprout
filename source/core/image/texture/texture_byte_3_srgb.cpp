#include "texture_byte_3_srgb.hpp"
#include "base/math/vector4.inl"
#include "image/typed_image.inl"
#include "texture_encoding.hpp"

namespace image::texture {

Byte3_sRGB::Byte3_sRGB(std::shared_ptr<Image> const& image) noexcept
    : Texture(image), image_(*static_cast<const Byte3*>(image.get())) {}

float Byte3_sRGB::at_1(int32_t i) const noexcept {
    auto const value = image_.load(i);
    return encoding::cached_srgb_to_float(value[0]);
}

float3 Byte3_sRGB::at_3(int32_t i) const noexcept {
    auto const value = image_.load(i);
    return float3(encoding::cached_srgb_to_float(value[0]),
                  encoding::cached_srgb_to_float(value[1]),
                  encoding::cached_srgb_to_float(value[2]));
}

float Byte3_sRGB::at_1(int32_t x, int32_t y) const noexcept {
    auto const value = image_.load(x, y);
    return encoding::cached_srgb_to_float(value[0]);
}

float2 Byte3_sRGB::at_2(int32_t x, int32_t y) const noexcept {
    auto const value = image_.load(x, y);
    return encoding::cached_srgb_to_float2(value);
}

float3 Byte3_sRGB::at_3(int32_t x, int32_t y) const noexcept {
    auto const value = image_.load(x, y);
    return encoding::cached_srgb_to_float3(value);
}

void Byte3_sRGB::gather_1(int4 const& xy_xy1, float c[4]) const noexcept {
    byte3 v[4];
    image_.gather(xy_xy1, v);

    encoding::cached_srgb_to_float(v, c);
}

void Byte3_sRGB::gather_2(int4 const& xy_xy1, float2 c[4]) const noexcept {
    byte3 v[4];
    image_.gather(xy_xy1, v);

    encoding::cached_srgb_to_float(v, c);
}

void Byte3_sRGB::gather_3(int4 const& xy_xy1, float3 c[4]) const noexcept {
    byte3 v[4];
    image_.gather(xy_xy1, v);

    encoding::cached_srgb_to_float(v, c);
}

float Byte3_sRGB::at_element_1(int32_t x, int32_t y, int32_t element) const noexcept {
    auto const value = image_.load_element(x, y, element);
    return encoding::cached_srgb_to_float(value[0]);
}

float2 Byte3_sRGB::at_element_2(int32_t x, int32_t y, int32_t element) const noexcept {
    auto const value = image_.load_element(x, y, element);
    return float2(encoding::cached_srgb_to_float(value[0]),
                  encoding::cached_srgb_to_float(value[1]));
}

float3 Byte3_sRGB::at_element_3(int32_t x, int32_t y, int32_t element) const noexcept {
    auto const value = image_.load_element(x, y, element);
    return encoding::cached_srgb_to_float3(value);
}

float Byte3_sRGB::at_1(int32_t x, int32_t y, int32_t z) const noexcept {
    auto const value = image_.load(x, y, z);
    return encoding::cached_srgb_to_float(value[0]);
}

float2 Byte3_sRGB::at_2(int32_t x, int32_t y, int32_t z) const noexcept {
    auto const value = image_.load(x, y, z);
    return float2(encoding::cached_srgb_to_float(value[0]),
                  encoding::cached_srgb_to_float(value[1]));
}

float3 Byte3_sRGB::at_3(int32_t x, int32_t y, int32_t z) const noexcept {
    auto const value = image_.load(x, y, z);
    return encoding::cached_srgb_to_float3(value);
}

}  // namespace image::texture
