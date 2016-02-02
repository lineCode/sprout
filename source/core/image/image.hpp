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
		Description(Type type, math::int2 dimensions, int32_t num_elements = 1);

		Type type;
		math::int2 dimensions;

		int32_t num_elements;
	};

	Image(const Description& description);
	virtual ~Image();

	const Description& description() const;

	void set_num_elements(int32_t num_elements);

protected:

	Description description_;

	int32_t area_;
};

}
