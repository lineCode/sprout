#pragma once

#include "distribution.hpp"

namespace math {

inline float scrambled_radical_inverse_vdC(uint32_t bits, uint32_t r) {
	bits = (bits << 16) | (bits >> 16);
	bits = ((bits & 0x55555555) << 1) | ((bits & 0xAAAAAAAA) >> 1);
	bits = ((bits & 0x33333333) << 2) | ((bits & 0xCCCCCCCC) >> 2);
	bits = ((bits & 0x0F0F0F0F) << 4) | ((bits & 0xF0F0F0F0) >> 4);
	bits = ((bits & 0x00FF00FF) << 8) | ((bits & 0xFF00FF00) >> 8);

	bits ^= r;

	return static_cast<float>(bits) * 2.3283064365386963e-10f; // / 0x100000000
}
/*
inline float scrambled_radical_inverse_S(uint32_t bits, uint32_t r) {
	for v := uint32(1 << 31); i != 0; i >>= 1 {
		if (i & 1) != 0 {
			r ^= v
		}

		v ^= v >> 1
	}

	return float32(r) * 2.3283064365386963e-10 // / 0x100000000
}
*/
inline float2 scrambled_hammersley(uint32_t i, uint32_t num_samples, uint32_t r) {
	return float2(static_cast<float>(i) / static_cast<float>(num_samples), scrambled_radical_inverse_vdC(i, r));
}

inline float2 ems(uint32_t i, uint32_t num_samples, uint32_t r) {

}

}
