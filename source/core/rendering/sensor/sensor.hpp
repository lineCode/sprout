#pragma once

#include "image/typed_image.hpp"
#include "base/math/rectangle.hpp"

namespace thread { class Pool; }

namespace sampler { struct Camera_sample; }

namespace rendering { namespace sensor {

class Sensor {

public:

	Sensor(int2 dimensions, float exposure);
	virtual ~Sensor();

	int2 dimensions() const;

	void resolve(thread::Pool& pool, image::Image_float_4& target) const;

	virtual int32_t filter_radius_int() const = 0;

	virtual void clear() = 0;

	virtual void add_sample(const sampler::Camera_sample& sample, const float4& color,
							const math::Recti& tile, const math::Recti& bounds) = 0;

	virtual size_t num_bytes() const = 0;

protected:

	virtual void add_pixel(int2 pixel, const float4& color, float weight) = 0;

	virtual void add_pixel_atomic(int2 pixel, const float4& color, float weight) = 0;

	virtual void resolve(int32_t begin, int32_t end, image::Image_float_4& target) const = 0;

	int2 dimensions_;

	float exposure_factor_;
};

}}
