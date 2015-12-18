#pragma once

#include "sensor.hpp"

namespace rendering { namespace sensor {

class Transparent : public Sensor {
public:

	Transparent(math::int2 dimensions, float exposure, std::unique_ptr<tonemapping::Tonemapper> tonemapper);
	virtual ~Transparent();

	virtual void clear() final override;

protected:

	virtual void add_pixel(math::int2 pixel, const math::float4& color, float weight) final override;

	virtual void add_pixel_atomic(math::int2 pixel, const math::float4& color, float weight) final override;

	virtual void resolve(int32_t begin, int32_t end) final override;

	struct Pixel {
		math::float4 color;
		float        weight_sum;
	};

	Pixel* pixels_;
};


}}