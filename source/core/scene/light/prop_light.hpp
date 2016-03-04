#pragma once

#include "light.hpp"

namespace scene {

namespace shape { class Node_stack; }

namespace light {

class Prop_light : public Light {
public:

	void init(Prop* prop, uint32_t part);

	virtual const entity::Composed_transformation& transformation_at(
			float time, entity::Composed_transformation& transformation) const final override;

	virtual void sample(const entity::Composed_transformation& transformation, float time,
						const math::float3& p, const math::float3& n, bool total_sphere,
						sampler::Sampler& sampler, Worker& worker,
						material::Texture_filter override_filter, Sample& result) const override;

	virtual float pdf(const entity::Composed_transformation& transformation,
					  const math::float3& p, const math::float3& wi, bool total_sphere,
					  Worker& worker, material::Texture_filter override_filter) const override;

	virtual math::float3 power(const math::aabb& scene_bb) const final override;

	virtual void prepare_sampling() override;

	virtual bool equals(const Prop* prop, uint32_t part) const final override;

protected:

	Prop* prop_;

	uint32_t part_;

	float area_;
};

}}
