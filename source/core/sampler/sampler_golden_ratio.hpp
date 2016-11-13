#pragma once

#include "sampler.hpp"
#include <vector>

namespace sampler {

class Golden_ratio : public Sampler {

public:

	Golden_ratio(rnd::Generator& rng, uint32_t num_samples);
	~Golden_ratio();

	virtual void generate_camera_sample(int2 pixel, uint32_t index,
										Camera_sample& sample) final override;

	virtual float2 generate_sample_2D() final override;

	virtual float generate_sample_1D() final override;

private:

	virtual void on_resume_pixel() final override;

	float2* samples_2D_;
	float*  samples_1D_;
};

class Golden_ratio_factory : public Factory {

public:

	Golden_ratio_factory(uint32_t num_samples);

	virtual Sampler* create(rnd::Generator& rng) const final override;
};

}