#include "postprocessor_pipeline.hpp"
#include "postprocessor.hpp"
#include "image/typed_image.inl"
#include "rendering/sensor/sensor.hpp"
#include "base/math/vector.inl"

namespace rendering { namespace postprocessor {

Pipeline::Pipeline() {}

Pipeline::~Pipeline() {
	for (auto pp : postprocessors_) {
		delete pp;
	}
}

void Pipeline::reserve(size_t num_pps) {
	postprocessors_.reserve(num_pps);
}

void Pipeline::add(Postprocessor* pp) {
	if (pp) {
		postprocessors_.push_back(pp);
	}
}

void Pipeline::init(const scene::camera::Camera& camera) {
	if (postprocessors_.empty()) {
		return;
	}

	image::Image::Description description(image::Image::Type::Float_4, camera.sensor_dimensions());
	scratch_.resize(description);

	for (auto pp : postprocessors_) {
		pp->init(camera);
	}
}

size_t Pipeline::num_bytes() const {
	size_t num_bytes = 0;
	for (auto pp : postprocessors_) {
		num_bytes += pp->num_bytes();
	}

	return num_bytes;
}

void Pipeline::apply(const sensor::Sensor& sensor, image::Image_float_4& target,
					 thread::Pool& pool) {
	if (postprocessors_.empty()) {
		sensor.resolve(pool, target);
	} else {
		image::Image_float_4* targets[2];

		if (0 == postprocessors_.size() % 2) {
			targets[0] = &target;
			targets[1] = &scratch_;
		} else {
			targets[0] = &scratch_;
			targets[1] = &target;
		}

		sensor.resolve(pool, *targets[0]);

		for (auto pp : postprocessors_) {
			pp->apply(*targets[0], *targets[1], pool);
			std::swap(targets[0], targets[1]);
		}
	}
}

}}
