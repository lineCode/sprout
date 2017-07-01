#pragma once

#include "shape/shape_intersection.hpp"
#include "material/sampler_settings.hpp"

namespace scene {

class Worker;
class Prop;

namespace material {

class BSSRDF;
class Sample;
class Material;

}

class Prop;

struct Intersection {
	using Sampler_filter = material::Sampler_settings::Filter;

	bool hit() const;

	material::Material* material() const;

	uint32_t light_id() const;

	float area() const;

	float opacity(float time, Worker& worker, Sampler_filter filter) const;

	float3 thin_absorption(const float3& wo, float time,
						   Worker& worker, Sampler_filter filter) const;

	const material::Sample& sample(const float3& wo, float time,
								   Worker& worker, Sampler_filter filter) const;

	const material::BSSRDF& bssrdf(Worker& worker) const;

	bool same_hemisphere(const float3& v) const;

	shape::Intersection geo;
	const Prop* prop;
};

}
