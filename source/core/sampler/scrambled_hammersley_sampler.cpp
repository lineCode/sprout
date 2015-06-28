#include "scrambled_hammersley_sampler.hpp"
#include "camera_sample.hpp"
#include "base/math/vector.inl"
#include "base/math/random/generator.inl"
#include "base/math/distribution.inl"

namespace sampler {

Scrambled_hammersley::Scrambled_hammersley(math::random::Generator& rng, uint32_t num_samples_per_iteration) :
	Sampler(rng, num_samples_per_iteration) {}

Sampler* Scrambled_hammersley::clone() const {
	return new Scrambled_hammersley(rng_, num_samples_per_iteration_);
}

void Scrambled_hammersley::generate_camera_sample(const math::float2& offset, uint32_t index, Camera_sample& sample) {
	math::float2 s2d = math::scrambled_hammersley(index, num_samples_per_iteration_, seed_);

	sample.coordinates = offset + s2d;
	sample.relative_offset = s2d - math::float2(0.5f, 0.5f);
	sample.lens_uv = s2d.yx();
	sample.time = rng_.random_float();
}

math::float2 Scrambled_hammersley::generate_sample_2d() {
	return math::scrambled_hammersley(current_sample_++, num_iterations_ * num_samples_per_iteration_, seed_);
}

float Scrambled_hammersley::generate_sample_1d() {
	return rng_.random_float();
}

uint32_t Scrambled_hammersley::seed() const {
	return rng_.random_uint();
}

}
