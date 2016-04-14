#pragma once

#include "distribution_1d.hpp"
#include "math/vector.hpp"

namespace math {

class Distribution_2D {
public:

	void init(const float* data, const math::int2& dimensions);

	math::float2 sample_continuous(math::float2 r2, float& pdf) const;

	float pdf(math::float2 uv) const;

private:

//	using Distribution_impl = Distribution_1D;
	using Distribution_impl = Distribution_lut_1D;

	Distribution_impl marginal_;

	std::vector<Distribution_impl> conditional_;

	float conditional_max_;
};

}
