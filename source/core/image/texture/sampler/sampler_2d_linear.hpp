#pragma once

#include "sampler_2d.hpp"

namespace image { namespace texture { namespace sampler {

template<typename Address_mode>
class Sampler_2D_linear : public Sampler_2D {
public:

	virtual float        sample_1(const Texture_2D& texture, math::float2 uv) const final override;
	virtual math::float2 sample_2(const Texture_2D& texture, math::float2 uv) const final override;
	virtual math::float3 sample_3(const Texture_2D& texture, math::float2 uv) const final override;

	virtual math::float2 address(math::float2 uv) const final override;

private:

	Address_mode address_mode_;
};

}}}
