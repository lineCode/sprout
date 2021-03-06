#ifndef SU_BASE_MATH_FOURIER_DFT_HPP
#define SU_BASE_MATH_FOURIER_DFT_HPP

#include "math/vector.hpp"

namespace thread {
class Pool;
}

namespace math {

int32_t dft_size(int32_t num);

void dft_1d(float2* result, float const* source, int32_t num);

void idft_1d(float* result, float2 const* source, int32_t num);

void dft_2d(float2* result, float const* source, int32_t width, int32_t height, thread::Pool& pool);

void dft_2d(float2* result, float const* source, float2* temp, int32_t width, int32_t height,
            thread::Pool& pool);

void idft_2d(float* result, float2 const* source, float2* temp, int32_t width, int32_t height,
             thread::Pool& pool);

}  // namespace math

#endif
