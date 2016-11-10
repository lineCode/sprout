#include "sampler_ld.hpp"
#include "camera_sample.hpp"
#include "base/math/vector.inl"
#include "base/math/sampling/sample_distribution.inl"
#include "base/random/generator.inl"

namespace sampler {

inline float2 ld(uint32_t i, uint32_t r0, uint32_t r1) {
	return float2(math::scrambled_radical_inverse_vdC(i, r0),
				  math::scrambled_radical_inverse_S(i, r1));
}

LD::LD(random::Generator& rng, uint32_t num_samples) :
	Sampler(rng, num_samples) {}

void LD::generate_camera_sample(int2 pixel, uint32_t index, Camera_sample& sample) {
	float2 s2d = ld(index, seed_.x, seed_.y);

	sample.pixel = pixel;
	sample.pixel_uv = s2d;
	sample.lens_uv = s2d.yx();
	sample.time = rng_.random_float();
}

float2 LD::generate_sample_2D() {
	return ld(current_sample_++, seed_.x, seed_.y);
}

float LD::generate_sample_1D() {
	return rng_.random_float();
}

LD_factory::LD_factory(uint32_t num_samples) :
	Factory(num_samples) {}

Sampler* LD_factory::create(random::Generator& rng) const {
	return new LD(rng, num_samples_);
}

}

