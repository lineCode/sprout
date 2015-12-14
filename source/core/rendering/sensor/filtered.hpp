#pragma once

#include "base/math/rectangle.hpp"
#include <memory>

namespace rendering { namespace sensor {

template<class Base, class Clamp, class Filter>
class Filtered : public Base {
public:

	Filtered(const math::int2& dimensions, float exposure,
			 std::unique_ptr<tonemapping::Tonemapper> tonemapper, const Clamp& clamp, std::unique_ptr<Filter> filter);

	~Filtered();

	virtual void add_sample(const sampler::Camera_sample& sample,
							const math::float4& color, const math::Recti& tile, const math::Recti& bounds) final override;

private:

	void weight_and_add_pixel(int32_t x, int32_t y, math::float2 relative_offset,
							  const math::float4& color, const math::Recti& tile, const math::Recti& bounds);

	Clamp clamp_;

	std::unique_ptr<Filter> filter_;
};

}}
