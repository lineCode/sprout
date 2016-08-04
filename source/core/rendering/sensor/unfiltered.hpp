#pragma once

#include "base/math/rectangle.hpp"
#include "base/math/vector.hpp"

namespace sampler { struct Camera_sample; }

namespace rendering { namespace sensor {

namespace tonemapping { class Tonemapper; }

template<class Base, class Clamp>
class Unfiltered : public Base {

public:

	Unfiltered(int2 dimensions, const tonemapping::Tonemapper* tonemapper, const Clamp& clamp);

	virtual int32_t filter_radius_int() const final override;

	virtual void add_sample(const sampler::Camera_sample& sample, const float4& color,
							const math::Recti& tile, const math::Recti& bounds) final override;

private:

	Clamp clamp_;
};

}}
