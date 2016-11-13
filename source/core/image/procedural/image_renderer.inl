#pragma once

#include "image_renderer.hpp"
#include "image/typed_image.inl"
#include "image/texture/sampler/address_mode.hpp"
#include "base/math/vector.inl"
#include "base/spectrum/rgb.inl"

namespace image { namespace procedural {

template<typename T>
Typed_renderer<T>::Typed_renderer(int2 dimensions, int32_t sqrt_num_samples) :
	sqrt_num_samples_(sqrt_num_samples),
	dimensions_(sqrt_num_samples * dimensions),
	dimensions_f_(sqrt_num_samples * dimensions),
	samples_(new T[sqrt_num_samples * dimensions.x * sqrt_num_samples * dimensions.y]),
	brush_(T(0))
{}

template<typename T>
Typed_renderer<T>::~Typed_renderer() {
	delete [] samples_;
}

template<typename T>
void Typed_renderer<T>::set_brush(T color) {
	brush_ = color;
}

template<typename T>
void Typed_renderer<T>::clear() {
	for (int32_t i = 0, len = dimensions_.x * dimensions_.y; i < len; ++i) {
		samples_[i] = brush_;
	}
}

template<typename T>
void Typed_renderer<T>::draw_circle(float2 pos, float radius) {
	int2 sample(pos * dimensions_f_);

	int32_t x = static_cast<int>(radius * dimensions_f_.x);
	int32_t y = 0;
	int32_t err = 0;

	while (x >= y) {
		set_row(sample.x - x, sample.x + x, sample.y + y, brush_);
		set_row(sample.x - y, sample.x + y, sample.y + x, brush_);
		set_row(sample.x - x, sample.x + x, sample.y - y, brush_);
		set_row(sample.x - y, sample.x + y, sample.y - x, brush_);

		y += 1;
		err += 1 + 2 * y;

		if (2 * (err - x) + 1 > 0) {
			x -= 1;
			err += 1 - 2 * x;
		}
	}
}

template<typename T>
void Typed_renderer<T>::resolve(Typed_image<T>& image) const {
	if (1 == sqrt_num_samples_) {
		for (int32_t i = 0, len = image.area(); i < len; ++i) {
			auto s = samples_[i];
			image.store(i, s);
		}
	} else {
		int32_t num_samples = sqrt_num_samples_ * sqrt_num_samples_;

		float n = 1.f / static_cast<float>(num_samples);

		auto i_d = image.description().dimensions;

		for (int32_t i_y = 0; i_y < i_d.y; ++i_y) {
			int32_t b_y = sqrt_num_samples_ * i_y;
			for (int32_t i_x = 0; i_x < i_d.x; ++i_x) {
				int32_t b_x = sqrt_num_samples_ * i_x;

				T result(0);

				for (int32_t y = 0; y < sqrt_num_samples_; ++y) {
					int32_t b_o = dimensions_.x * (b_y + y) + b_x;
					for (int32_t x = 0; x < sqrt_num_samples_; ++x) {
						int32_t s = b_o + x;
						result += samples_[s];
					}
				}

				image.store(i_x, i_y, n * result);
			}
		}
	}
}

template<typename T>
void Typed_renderer<T>::set_sample(int32_t x, int32_t y, T color) {
	x = math::mod(x, dimensions_.x);
	y = math::mod(y, dimensions_.y);

	samples_[dimensions_.x * y + x] = color;
}

template<typename T>
void Typed_renderer<T>::set_row(int32_t start_x, int32_t end_x, int32_t y, T color) {
	for (int32_t x = start_x; x < end_x; ++x) {
		set_sample(x, y, color);
	}
}

}}