#pragma once

#include "scene/material/material.hpp"
#include "base/math/bounding/aabb.hpp"

namespace sampler { class Sampler; }

namespace scene {

class Worker;

namespace entity { struct Composed_transformation; }

namespace shape {

struct Intersection;
struct Sample;
class Node_stack;
class Morphable_shape;

class Shape {
public:

	using Entity_transformation = entity::Composed_transformation;
	using Sampler_filter = material::Sampler_settings::Filter;

	virtual ~Shape();

	const math::aabb& aabb() const;

	virtual uint32_t num_parts() const;

	virtual bool intersect(const Entity_transformation& transformation, math::Oray& ray,
						   Node_stack& node_stack, Intersection& intersection) const = 0;

	virtual bool intersect_p(const Entity_transformation& transformation, const math::Oray& ray,
							 Node_stack& node_stack) const = 0;

	virtual float opacity(const Entity_transformation& transformation, const math::Oray& ray,
						  float time, const material::Materials& materials,
						  Worker& worker, Sampler_filter filter) const = 0;

	virtual void sample(uint32_t part, const Entity_transformation& transformation, float area,
						const math::float3& p, const math::float3& n,
						bool two_sided, sampler::Sampler& sampler,
						Node_stack& node_stack, Sample& sample) const = 0;

	virtual void sample(uint32_t part, const Entity_transformation& transformation, float area,
						const math::float3& p, bool two_sided, sampler::Sampler& sampler,
						Node_stack& node_stack, Sample& sample) const = 0;

	virtual void sample(uint32_t part, const Entity_transformation& transformation, float area,
						const math::float3& p, math::float2 uv, Sample& sample) const = 0;

	virtual void sample(uint32_t part, const Entity_transformation& transformation, float area,
						const math::float3& p, const math::float3& wi, Sample& sample) const = 0;

	virtual float pdf(uint32_t part, const Entity_transformation& transformation, float area,
					  const math::float3& p, const math::float3& wi, bool two_sided,
					  bool total_sphere, Node_stack& node_stack) const = 0;

	virtual float area(uint32_t part, const math::float3& scale) const = 0;

	virtual bool is_complex() const;
	virtual bool is_finite() const;
	virtual bool is_analytical() const;

	virtual void prepare_sampling(uint32_t part);

	virtual Morphable_shape* morphable_shape();

protected:

	math::aabb aabb_;
};

}}
