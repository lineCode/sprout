#ifndef SU_CORE_IMAGE_TYPED_IMAGE_INL
#define SU_CORE_IMAGE_TYPED_IMAGE_INL

#include "base/math/vector4.inl"
#include "base/memory/align.hpp"
#include "typed_image.hpp"
#include "typed_image_fwd.hpp"

namespace image {

template <typename T>
Typed_image<T>::Typed_image(const Image::Description& description) noexcept
    : Image(description), data_(memory::allocate_aligned<T>(description.num_pixels())) {}

template <typename T>
Typed_image<T>::~Typed_image() noexcept {
    memory::free_aligned(data_);
}

template <typename T>
Typed_image<T> Typed_image<T>::clone() const noexcept {
    return Typed_image<T>(description_);
}

template <typename T>
void Typed_image<T>::resize(const Image::Description& description) noexcept {
    memory::free_aligned(data_);

    Image::resize(description);

    data_ = memory::allocate_aligned<T>(description.num_pixels());
}

template <typename T>
void Typed_image<T>::clear(T v) noexcept {
    for (int32_t i = 0, len = volume(); i < len; ++i) {
        data_[i] = v;
    }
}

template <typename T>
T Typed_image<T>::load(int32_t index) const noexcept {
    return data_[index];
}

template <typename T>
T* Typed_image<T>::address(int32_t index) const noexcept {
    return data_ + index;
}

template <typename T>
void Typed_image<T>::store(int32_t index, T v) noexcept {
    data_[index] = v;
}

template <typename T>
T const& Typed_image<T>::at(int32_t index) const noexcept {
    return data_[index];
}

template <typename T>
T Typed_image<T>::load(int32_t x, int32_t y) const noexcept {
    int32_t const i = y * description_.dimensions[0] + x;
    return data_[i];
}

template <typename T>
void Typed_image<T>::store(int32_t x, int32_t y, T v) noexcept {
    int32_t const i = y * description_.dimensions[0] + x;
    data_[i]        = v;
}

template <typename T>
T Typed_image<T>::load_element(int32_t x, int32_t y, int32_t element) const noexcept {
    int32_t const i = (element * description_.dimensions[1] + y) * description_.dimensions[0] + x;
    return data_[i];
}

template <typename T>
T const& Typed_image<T>::at(int32_t x, int32_t y) const noexcept {
    int32_t const i = y * description_.dimensions[0] + x;
    return data_[i];
}

template <typename T>
T const& Typed_image<T>::at_element(int32_t x, int32_t y, int32_t element) const noexcept {
    int32_t const i = (element * description_.dimensions[1] + y) * description_.dimensions[0] + x;
    return data_[i];
}

template <typename T>
T Typed_image<T>::load(int32_t x, int32_t y, int32_t z) const noexcept {
    int32_t const i = (z * description_.dimensions[1] + y) * description_.dimensions[0] + x;
    return data_[i];
}

template <typename T>
T const& Typed_image<T>::at(int32_t x, int32_t y, int32_t z) const noexcept {
    int32_t const i = (z * description_.dimensions[1] + y) * description_.dimensions[0] + x;
    return data_[i];
}

template <typename T>
T const& Typed_image<T>::at_element(int32_t x, int32_t y, int32_t z, int32_t element) const
    noexcept {
    int3 const    d = description_.dimensions;
    int32_t const i = ((element * d[2] + z) * d[1] + y) * d[0] + x;
    return data_[i];
}

template <typename T>
void Typed_image<T>::gather(int4 const& xy_xy1, T c[4]) const noexcept {
    int32_t const width = description_.dimensions[0];

    int32_t const y0 = width * xy_xy1[1];

    c[0] = data_[y0 + xy_xy1[0]];
    c[1] = data_[y0 + xy_xy1[2]];

    int32_t const y1 = width * xy_xy1[3];

    c[2] = data_[y1 + xy_xy1[0]];
    c[3] = data_[y1 + xy_xy1[2]];
}

template <typename T>
void Typed_image<T>::square_transpose() noexcept {
    int32_t const n = description_.dimensions[0];
    for (int32_t y = 0, height = n - 2; y < height; ++y) {
        for (int32_t x = y + 1, width = n - 1; x < width; ++x) {
            int32_t const a = y * n + x;
            int32_t const b = x * n + y;
            std::swap(data_[a], data_[b]);
        }
    }
}

template <typename T>
T* Typed_image<T>::data() const noexcept {
    return data_;
}

template <typename T>
size_t Typed_image<T>::num_bytes() const noexcept {
    return sizeof(*this) + description_.dimensions[0] * description_.dimensions[1] *
                               description_.dimensions[2] * description_.num_elements * sizeof(T);
}

}  // namespace image

#endif
