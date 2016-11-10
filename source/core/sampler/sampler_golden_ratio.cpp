#include "sampler_golden_ratio.hpp"
#include "camera_sample.hpp"
#include "base/math/vector.inl"
#include "base/math/sampling/sample_distribution.inl"
#include "base/random/generator.inl"
#include "base/random/shuffle.inl"

#include "base/debug/assert.hpp"

namespace sampler {

Golden_ratio::Golden_ratio(random::Generator& rng,
						   uint32_t num_samples) :
	Sampler(rng, num_samples),
	samples_(new float2[num_samples]) {}

Golden_ratio::~Golden_ratio() {
	delete [] samples_;
}

void Golden_ratio::generate_camera_sample(int2 pixel, uint32_t index,
										  Camera_sample& sample) {
	float2 s2d = samples_[index];

	sample.pixel = pixel;
	sample.pixel_uv = s2d;
	sample.lens_uv = s2d.yx();
	sample.time = rng_.random_float();
}

float2 Golden_ratio::generate_sample_2D() {
	SOFT_ASSERT(current_sample_ < num_samples_);

	return samples_[current_sample_++];
}

float Golden_ratio::generate_sample_1D() {
	return rng_.random_float();
}

void Golden_ratio::on_resume_pixel() {
	float2 r(rng_.cast(seed_.x), rng_.cast(seed_.y));
	math::golden_ratio(samples_, num_samples_, r);

//	std::random_device rd;
//	std::mt19937 g(seed_.x);
//	std::shuffle(samples_.begin(), samples_.end(), g);

	random::Generator rng(seed_.x + 0, seed_.x + 1, seed_.y + 2, seed_.y + 3);
	random::shuffle(samples_, num_samples_, rng);
}

Golden_ratio_factory::Golden_ratio_factory(uint32_t num_samples) :
	Factory(num_samples) {}

Sampler* Golden_ratio_factory::create(random::Generator& rng) const {
	return new Golden_ratio(rng, num_samples_);
}

}
