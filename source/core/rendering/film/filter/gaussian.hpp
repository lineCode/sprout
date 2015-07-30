#pragma once

#include "base/math/vector.hpp"
#include "base/math/function/interpolated_function.hpp"

namespace rendering { namespace film { namespace filter {

class Gaussian {
public:

	Gaussian(float radius, float alpha);

	float evaluate(math::float2 p) const;

private:

	float gaussian(float d) const;

	float exp_;

	float alpha_;

	math::Interpolated_function<float> gaussian_;
};

}}}
