#pragma once

#include "prop_light.hpp"

namespace scene { namespace light {

class Prop_image_light : public Prop_light {
public:

	virtual void sample(const entity::Composed_transformation& transformation, const math::float3& p, const math::float3& n,
						const image::sampler::Sampler_2D& image_sampler, sampler::Sampler& sampler,
						uint32_t max_samples, std::vector<Sample>& samples) const final override;

	virtual float pdf(const entity::Composed_transformation& transformation, const math::float3& p, const math::float3& wi) const final override;

	virtual void prepare_sampling() final override;
};

}}

