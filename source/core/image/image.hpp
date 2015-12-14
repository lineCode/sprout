#pragma once

#include "base/math/vector.hpp"

namespace image {

class Image {
public:

	enum class Type {
		Byte_1,
		Byte_2,
		Byte_3,
		Float_3,
		Float_4
	};

	struct Description {
		Description();
		Description(Type type, math::int2 dimensions);

		Type type;
		math::int2 dimensions;
	};

	Image(const Description& description);
	virtual ~Image();

	const Description& description() const;

	virtual void resize(math::int2 dimensions) = 0;

protected:

	Description description_;
};

}
