#pragma once

#include "base/math/vector.hpp"

namespace rnd { class Generator; }

namespace sampler {

struct Camera_sample;

class Sampler {

public:

	Sampler(rnd::Generator& rng, uint32_t num_samples);
	virtual ~Sampler();

	rnd::Generator& rng();

	uint32_t num_samples() const;

	void resume_pixel(uint32_t sample, rnd::Generator& scramble);

	virtual void generate_camera_sample(int2 pixel, uint32_t index, Camera_sample& sample) = 0;

	virtual float2 generate_sample_2D() = 0;

	virtual float generate_sample_1D() = 0;

protected:

	virtual void on_resume_pixel(rnd::Generator& scramble);

	rnd::Generator& rng_;
	uint32_t num_samples_;
	uint32_t current_sample_2D_;
	uint32_t current_sample_1D_;
};

class Factory {

public:

	Factory(uint32_t num_samples);

	virtual Sampler* create(rnd::Generator& rng) const = 0;

	uint32_t num_samples_per_iteration() const;

protected:

	uint32_t num_samples_;
};

}
