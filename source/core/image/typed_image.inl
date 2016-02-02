#pragma once

#include "typed_image.hpp"

namespace image {

template<typename T>
Typed_image<T>::Typed_image(const Image::Description& description) :
	Image(description),
	data_(new T[description.dimensions.x * description.dimensions.y * description.num_elements]) {}

template<typename T>
Typed_image<T>::~Typed_image() {
	delete [] data_;
}

template<typename T>
const T& Typed_image<T>::at(int32_t index) const {
	return data_[index];
}

template<typename T>
T& Typed_image<T>::at(int32_t index) {
	return data_[index];
}

template<typename T>
const T& Typed_image<T>::at(int32_t x, int32_t y) const {
	int32_t i = y * description_.dimensions.x + x;
	return data_[i];
}

template<typename T>
T& Typed_image<T>::at(int32_t x, int32_t y) {
	int32_t i = y * description_.dimensions.x + x;
	return data_[i];
}

template<typename T>
const T& Typed_image<T>::at(int32_t x, int32_t y, int32_t element) const {
	int32_t i = element * area_ + y * description_.dimensions.x + x;
	return data_[i];
}

template<typename T>
T& Typed_image<T>::at(int32_t x, int32_t y, int32_t element) {
	int32_t i = element * area_ + y * description_.dimensions.x + x;
	return data_[i];
}

}
