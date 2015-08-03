#pragma once

#include "sampler.hpp"

namespace sampler {

class LD : public Sampler {
public:

	LD(math::random::Generator& rng, uint32_t num_samples_per_iteration);

	virtual Sampler* clone() const final override;

	virtual void generate_camera_sample(math::uint2 pixel, uint32_t index, Camera_sample& sample) final override;

	virtual math::float2 generate_sample_2D() final override;

	virtual float generate_sample_1D() final override;

private:

	virtual math::uint2 seed() const final override;
};

}
