#include "sampler_ld.hpp"
#include "camera_sample.hpp"
#include "base/math/vector.inl"
#include "base/math/sampling/sample_distribution.inl"
#include "base/random/generator.inl"

namespace sampler {

inline float2 ld(uint32_t i, uint32_t r0, uint32_t r1) {
	return float2(math::radical_inverse_vdC(i, r0),
				  math::radical_inverse_S(i, r1));
}

LD::LD(rnd::Generator& rng, uint32_t num_samples) :
	Sampler(rng, num_samples) {}

void LD::generate_camera_sample(int2 pixel, uint32_t index, Camera_sample& sample) {
	float2 s2d = ld(index, scramble_.x, scramble_.y);

	sample.pixel = pixel;
	sample.pixel_uv = s2d;
	sample.lens_uv = s2d.yx();
	sample.time = rng_.random_float();
}

float2 LD::generate_sample_2D() {
	return ld(current_sample_2D_++, scramble_.x, scramble_.y);
}

float LD::generate_sample_1D() {
	return rng_.random_float();
}

void LD::on_resume_pixel(rnd::Generator& scramble) {
	scramble_ = uint2(scramble.random_uint(), scramble.random_uint());
}

LD_factory::LD_factory(uint32_t num_samples) :
	Factory(num_samples) {}

Sampler* LD_factory::create(rnd::Generator& rng) const {
	return new LD(rng, num_samples_);
}

}

