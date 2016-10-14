#include "image.hpp"
#include "base/math/vector.inl"

namespace image {

Image::Description::Description() :
	type(Type::Unknown),
	dimensions(0, 0, 0),
	num_elements(0) {}

Image::Description::Description(Type type, int2 dimensions, int32_t num_elements) :
	type(type),
	dimensions(dimensions, 1),
	num_elements(num_elements) {}

Image::Description::Description(Type type, const int3& dimensions, int32_t num_elements) :
	type(type),
	dimensions(dimensions),
	num_elements(num_elements) {}

Image::Image() : area_(0), volume_(0) {}

Image::Image(const Description& description) :
	description_(description),
	area_(description.dimensions.x * description.dimensions.y),
	volume_(description.dimensions.x * description.dimensions.y * description.dimensions.z) {}

Image::~Image() {}

void Image::resize(const Description &description) {
	description_ = description;
	area_ = description.dimensions.x * description.dimensions.y;
	volume_ = description.dimensions.x * description.dimensions.y * description.dimensions.z;
}

const Image::Description& Image::description() const {
	return description_;
}

int32_t Image::area() const {
	return area_;
}

int32_t Image::volume() const {
	return volume_;
}

}
