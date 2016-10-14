#pragma once

#include "image/typed_image.hpp"
#include "scene/camera/camera.hpp"

namespace thread { class Pool; }

namespace rendering { namespace postprocessor {

class Postprocessor {

public:

	virtual ~Postprocessor();

	virtual void init(const scene::camera::Camera& camera) = 0;

	void apply(const image::Image_float_4& source,
			   image::Image_float_4& destination,
			   thread::Pool& pool) const;

private:

	virtual void apply(int32_t begin, int32_t end,
					   const image::Image_float_4& source,
					   image::Image_float_4& destination) const = 0;

};

}}

