#pragma once

#include "image/typed_image_fwd.hpp"
#include "texture.hpp"

namespace image::texture {

class Byte2_unorm final : public Texture {
  public:
    Byte2_unorm(std::shared_ptr<Image> image) noexcept;

    float  at_1(int32_t i) const noexcept override final;
    float3 at_3(int32_t i) const noexcept override final;

    float  at_1(int32_t x, int32_t y) const noexcept override final;
    float2 at_2(int32_t x, int32_t y) const noexcept override final;
    float3 at_3(int32_t x, int32_t y) const noexcept override final;

    void gather_1(int4 const& xy_xy1, float c[4]) const noexcept override final;
    void gather_2(int4 const& xy_xy1, float2 c[4]) const noexcept override final;
    void gather_3(int4 const& xy_xy1, float3 c[4]) const noexcept override final;

    float  at_element_1(int32_t x, int32_t y, int32_t element) const noexcept override final;
    float2 at_element_2(int32_t x, int32_t y, int32_t element) const noexcept override final;
    float3 at_element_3(int32_t x, int32_t y, int32_t element) const noexcept override final;

    float  at_1(int32_t x, int32_t y, int32_t z) const noexcept override final;
    float2 at_2(int32_t x, int32_t y, int32_t z) const noexcept override final;
    float3 at_3(int32_t x, int32_t y, int32_t z) const noexcept override final;

  private:
    Byte2 const& image_;
};

}  // namespace image::texture
