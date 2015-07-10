#pragma once

#include "base/math/vector.hpp"

namespace image {

class Image {
public:

	struct Description {
		Description();
		Description(math::uint2 dimensions);

		math::uint2 dimensions;
	};

	Image(const Description& description);
	virtual ~Image();

	const Description& description() const;

	virtual float        at1(uint32_t index) const = 0;
	virtual math::float2 at2(uint32_t index) const = 0;
	virtual math::float3 at3(uint32_t index) const = 0;
	virtual math::float4 at4(uint32_t index) const = 0;

	virtual void set1(uint32_t index, float value) = 0;
	virtual void set3(uint32_t index, const math::float3& value) = 0;
	virtual void set4(uint32_t index, const math::float4& value) = 0;

protected:

	Description description_;
};

}
