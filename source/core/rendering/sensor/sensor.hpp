#ifndef SU_CORE_RENDERING_SENSOR_SENSOR_HPP
#define SU_CORE_RENDERING_SENSOR_SENSOR_HPP

#include <cstddef>
#include "base/math/vector2.hpp"
#include "image/typed_image_fwd.hpp"

namespace thread {
class Pool;
}

namespace sampler {
struct Camera_sample;
}

namespace rendering::sensor {

class Sensor {
  public:
    Sensor(int2 dimensions, float exposure) noexcept;

    virtual ~Sensor() noexcept;

    int2 dimensions() const noexcept;

    void resolve(thread::Pool& pool, image::Float4& target) const noexcept;

    virtual int32_t filter_radius_int() const noexcept = 0;

    virtual int4 isolated_tile(int4 const& tile) const noexcept = 0;

    virtual void clear() = 0;

    virtual void add_sample(sampler::Camera_sample const& sample, float4 const& color,
                            int4 const& isolated_bounds, int4 const& bounds) noexcept = 0;

    virtual bool has_alpha_transparency() const noexcept = 0;

    virtual size_t num_bytes() const noexcept = 0;

  protected:
    virtual void add_pixel(int2 pixel, float4 const& color, float weight) noexcept = 0;

    virtual void add_pixel_atomic(int2 pixel, float4 const& color, float weight) noexcept = 0;

    virtual void resolve(int32_t begin, int32_t end, image::Float4& target) const noexcept = 0;

    int2 dimensions_;

    float exposure_factor_;
};

}  // namespace rendering::sensor

#endif
