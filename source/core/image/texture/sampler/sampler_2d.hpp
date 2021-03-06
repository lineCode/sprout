#ifndef SU_CORE_IMAGE_TEXTURE_SAMPLER_2D
#define SU_CORE_IMAGE_TEXTURE_SAMPLER_2D

#include "base/math/vector.hpp"

namespace image::texture {

class Texture;

namespace sampler {

class Sampler_2D {
  public:
    virtual ~Sampler_2D() {}

    virtual float  sample_1(Texture const& texture, float2 uv) const noexcept = 0;
    virtual float2 sample_2(Texture const& texture, float2 uv) const noexcept = 0;
    virtual float3 sample_3(Texture const& texture, float2 uv) const noexcept = 0;

    virtual float  sample_1(Texture const& texture, float2 uv, int32_t element) const noexcept = 0;
    virtual float2 sample_2(Texture const& texture, float2 uv, int32_t element) const noexcept = 0;
    virtual float3 sample_3(Texture const& texture, float2 uv, int32_t element) const noexcept = 0;

    virtual float2 address(float2 uv) const noexcept = 0;
};

}  // namespace sampler
}  // namespace image::texture

#endif
