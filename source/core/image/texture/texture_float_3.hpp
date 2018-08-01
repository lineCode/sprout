#ifndef SU_CORE_IMAGE_TEXTURE_FLOAT3_HPP
#define SU_CORE_IMAGE_TEXTURE_FLOAT3_HPP

#include "image/typed_image_fwd.hpp"
#include "texture.hpp"

namespace image::texture {

class Float3 final : public Texture {
  public:
    Float3(std::shared_ptr<Image> const& image);

    virtual float  at_1(int32_t i) const override final;
    virtual float3 at_3(int32_t i) const override final;

    virtual float  at_1(int32_t x, int32_t y) const override final;
    virtual float2 at_2(int32_t x, int32_t y) const override final;
    virtual float3 at_3(int32_t x, int32_t y) const override final;
    virtual float4 at_4(int32_t x, int32_t y) const override final;

    virtual float4 gather_1(int4 const& xy_xy1) const override final;
    virtual void   gather_2(int4 const& xy_xy1, float2 c[4]) const override final;
    virtual void   gather_3(int4 const& xy_xy1, float3 c[4]) const override final;

    virtual float  at_element_1(int32_t x, int32_t y, int32_t element) const override final;
    virtual float2 at_element_2(int32_t x, int32_t y, int32_t element) const override final;
    virtual float3 at_element_3(int32_t x, int32_t y, int32_t element) const override final;
    virtual float4 at_element_4(int32_t x, int32_t y, int32_t element) const override final;

    virtual float  at_1(int32_t x, int32_t y, int32_t z) const override final;
    virtual float2 at_2(int32_t x, int32_t y, int32_t z) const override final;
    virtual float3 at_3(int32_t x, int32_t y, int32_t z) const override final;
    virtual float4 at_4(int32_t x, int32_t y, int32_t z) const override final;

  private:
    image::Float3 const& image_;
};

}  // namespace image::texture

#endif
