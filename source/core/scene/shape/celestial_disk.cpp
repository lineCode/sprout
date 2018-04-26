#include "celestial_disk.hpp"
#include "shape_sample.hpp"
#include "shape_intersection.hpp"
#include "scene/scene_constants.hpp"
#include "scene/scene_ray.inl"
#include "scene/entity/composed_transformation.hpp"
#include "sampler/sampler.hpp"
#include "base/math/aabb.inl"
#include "base/math/vector3.inl"
#include "base/math/matrix3x3.inl"
#include "base/math/sampling/sampling.hpp"

namespace scene::shape {

Celestial_disk::Celestial_disk() {
	aabb_.set_min_max(float3::identity(), float3::identity());
}

bool Celestial_disk::intersect(Ray& ray, const Transformation& transformation,
							   Node_stack& /*node_stack*/, Intersection& intersection) const {
	const float3& n = transformation.rotation.r[2];
	const float b = math::dot(n, ray.direction);

	if (b > 0.f) {
		return false;
	}

	const float radius = transformation.scale[0];
	const float det = (b * b) - math::dot(n, n) + (radius * radius);

	if (det > 0.f && ray.max_t >= Ray_max_t) {
		constexpr float hit_t = Almost_ray_max_t;
		ray.max_t = hit_t;

		intersection.p = ray.point(hit_t);
		intersection.geo_n = n;
		intersection.t = transformation.rotation.r[0];
		intersection.b = transformation.rotation.r[1];
		intersection.n = n;

		const float3 k = ray.direction - n;
		const float3 sk = k / radius;
		intersection.uv[0] = (math::dot(intersection.t, sk) + 1.f) * 0.5f;
		intersection.uv[1] = (math::dot(intersection.b, sk) + 1.f) * 0.5f;

		intersection.epsilon = 5e-4f;
		intersection.part = 0;

		return true;
	}

	return false;
}

bool Celestial_disk::intersect_fast(Ray& ray, const Transformation& transformation,
									Node_stack& /*node_stack*/, Intersection& intersection) const {
	const float3& n = transformation.rotation.r[2];
	const float b = math::dot(n, ray.direction);

	if (b > 0.f) {
		return false;
	}

	const float radius = transformation.scale[0];
	const float det = (b * b) - math::dot(n, n) + (radius * radius);

	if (det > 0.f && ray.max_t >= Ray_max_t) {
		constexpr float hit_t = Almost_ray_max_t;
		ray.max_t = hit_t;

		intersection.p = ray.point(hit_t);
		intersection.geo_n = n;

		const float3 k = ray.direction - n;
		const float3 sk = k / radius;
		intersection.uv[0] = (math::dot(intersection.t, sk) + 1.f) * 0.5f;
		intersection.uv[1] = (math::dot(intersection.b, sk) + 1.f) * 0.5f;

		intersection.epsilon = 5e-4f;
		intersection.part = 0;

		return true;
	}

	return false;
}

bool Celestial_disk::intersect(Ray& ray, const Transformation& transformation,
							   Node_stack& /*node_stack*/, float& epsilon) const {
	const float3& n = transformation.rotation.r[2];
	const float b = math::dot(n, ray.direction);

	if (b > 0.f) {
		return false;
	}

	const float radius = transformation.scale[0];
	const float det = (b * b) - math::dot(n, n) + (radius * radius);

	if (det > 0.f && ray.max_t >= Ray_max_t) {
		ray.max_t = Almost_ray_max_t;
		epsilon = 5e-4f;
		return true;
	}

	return false;
}

bool Celestial_disk::intersect_p(const Ray& ray, const Transformation& transformation,
								 Node_stack& /*node_stack*/) const {
	const float3& n = transformation.rotation.r[2];
	const float b = math::dot(n, ray.direction);

	if (b > 0.f) {
		return false;
	}

	const float radius = transformation.scale[0];
	const float det = (b * b) - math::dot(n, n) + (radius * radius);

	if (det > 0.f && ray.max_t >= Ray_max_t) {
		return true;
	}

	return false;
}

float Celestial_disk::opacity(const Ray& /*ray*/, const Transformation& /*transformation*/,
							  const Materials& /*materials*/, Sampler_filter /*filter*/,
							  const Worker& /*worker*/) const {
	// Implementation for this is not really needed, so just skip it
	return 0.f;
}

float3 Celestial_disk::thin_absorption(const Ray& /*ray*/, const Transformation& /*transformation*/,
									   const Materials& /*materials*/, Sampler_filter /*filter*/,
									   const Worker& /*worker*/) const {
	// Implementation for this is not really needed, so just skip it
	return float3(0.f);
}

bool Celestial_disk::sample(uint32_t part, f_float3 p, f_float3 /*n*/,
							const Transformation& transformation, float area, bool two_sided,
							sampler::Sampler& sampler, uint32_t sampler_dimension,
							Node_stack& node_stack, Sample& sample) const {
	return Celestial_disk::sample(part, p, transformation, area, two_sided,
								  sampler, sampler_dimension, node_stack, sample);
}

bool Celestial_disk::sample(uint32_t /*part*/, f_float3 /*p*/,
							const Transformation& transformation, float area, bool /*two_sided*/,
							sampler::Sampler& sampler, uint32_t sampler_dimension,
							Node_stack& /*node_stack*/, Sample& sample) const {
	float2 r2 = sampler.generate_sample_2D(sampler_dimension);
	float2 xy = math::sample_disk_concentric(r2);

	float3 ls = float3(xy, 0.f);
	float radius = transformation.scale[0];
	float3 ws = radius * math::transform_vector(ls, transformation.rotation);

	sample.wi = math::normalize(ws - transformation.rotation.r[2]);

	sample.pdf = 1.f / area;
	sample.t = Almost_ray_max_t;
	sample.epsilon = 5e-4f;

	return true;
}

float Celestial_disk::pdf(const Ray& /*ray*/, const shape::Intersection& /*intersection*/,
						  const Transformation& /*transformation*/,
						  float area, bool /*two_sided*/, bool /*total_sphere*/) const {
	return 1.f / area;
}

bool Celestial_disk::sample(uint32_t /*part*/, f_float3 /*p*/, float2 /*uv*/,
							const Transformation& /*transformation*/, float /*area*/,
							bool /*two_sided*/, Sample& /*sample*/) const {
	return false;
}

float Celestial_disk::pdf_uv(const Ray& /*ray*/, const Intersection& /*intersection*/,
							 const Transformation& /*transformation*/,
							 float area, bool /*two_sided*/) const {
	return 1.f / area;
}

float Celestial_disk::uv_weight(float2 /*uv*/) const {
	return 1.f;
}

float Celestial_disk::area(uint32_t /*part*/, f_float3 scale) const {
	const float radius = scale[0];
	return math::Pi * (radius * radius);
}

bool Celestial_disk::is_finite() const {
	return false;
}

size_t Celestial_disk::num_bytes() const {
	return sizeof(*this);
}

}
