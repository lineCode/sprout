#include "sphere.hpp"
#include "shape_sample.hpp"
#include "shape_intersection.hpp"
#include "scene/scene_ray.inl"
#include "scene/scene_worker.hpp"
#include "scene/entity/composed_transformation.hpp"
#include "sampler/sampler.hpp"
#include "base/math/aabb.inl"
#include "base/math/vector3.inl"
#include "base/math/matrix3x3.inl"
#include "base/math/sampling/sampling.hpp"

#include "shape_test.hpp"
#include "base/debug/assert.hpp"

namespace scene::shape {

Sphere::Sphere() {
	aabb_.set_min_max(float3(-1.f), float3(1.f));
}

math::AABB Sphere::transformed_aabb(const float4x4& /*m*/, const math::Transformation& t) const {
	return transformed_aabb(t);
}

math::AABB Sphere::transformed_aabb(const math::Transformation& t) const {
	const float3 halfsize(t.scale[0]);
	return math::AABB(t.position - halfsize, t.position + halfsize);
}

bool Sphere::intersect(const Transformation& transformation, Ray& ray,
					   Node_stack& /*node_stack*/, Intersection& intersection) const {
	float3 v = transformation.position - ray.origin;
	float b = math::dot(v, ray.direction);
	float radius = transformation.scale[0];
	float det = (b * b) - math::dot(v, v) + (radius * radius);

	if (det > 0.f) {
		float dist = std::sqrt(det);
		float t0 = b - dist;

		if (t0 > ray.min_t && t0 < ray.max_t) {
			intersection.epsilon = 5e-4f * t0;

			float3 p = ray.point(t0);
			float3 n = math::normalize(p - transformation.position);

			float3 xyz = math::transform_vector_transposed(n, transformation.rotation);
			xyz = math::normalize(xyz);

			float phi   = -std::atan2(xyz[0], xyz[2]) + math::Pi;
			float theta = std::acos(xyz[1]);

			// avoid singularity at poles
			float sin_theta = std::max(std::sin(theta), 0.00001f);
			float sin_phi   = std::sin(phi);
			float cos_phi   = std::cos(phi);

			float3 t(sin_theta * cos_phi, 0.f, sin_theta * sin_phi);
			t = math::normalize(math::transform_vector(t, transformation.rotation));

			intersection.p = p;
			intersection.t = t;
			intersection.b = -math::cross(t, n);
			intersection.n = n;
			intersection.geo_n = n;
			intersection.uv = float2(phi * (0.5f * math::Pi_inv), theta * math::Pi_inv);
			intersection.part = 0;

			SOFT_ASSERT(testing::check(intersection, transformation, ray));

			ray.max_t = t0;
			return true;
		}

		float t1 = b + dist;

		if (t1 > ray.min_t && t1 < ray.max_t) {
			intersection.epsilon = 5e-4f * t1;

			float3 p = ray.point(t1);
			float3 n = math::normalize(p - transformation.position);

			float3 xyz = math::transform_vector_transposed(n, transformation.rotation);
			xyz = math::normalize(xyz);

			float phi   = -std::atan2(xyz[0], xyz[2]) + math::Pi;
			float theta = std::acos(xyz[1]);

			// avoid singularity at poles
			float sin_theta = std::max(std::sin(theta), 0.00001f);
			float sin_phi   = std::sin(phi);
			float cos_phi   = std::cos(phi);

			float3 t(sin_theta * cos_phi, 0.f, sin_theta * sin_phi);
			t = math::normalize(math::transform_vector(t, transformation.rotation));

			intersection.p = p;
			intersection.t = t;
			intersection.b = -math::cross(t, n);
			intersection.n = n;
			intersection.geo_n = n;
			intersection.uv = float2(phi * (0.5f * math::Pi_inv), theta * math::Pi_inv);
			intersection.part = 0;

			SOFT_ASSERT(testing::check(intersection, transformation, ray));

			ray.max_t = t1;
			return true;
		}
	}

	return false;
}

bool Sphere::intersect(const Transformation& transformation, Ray& ray,
					   Node_stack& /*node_stack*/, float& epsilon, bool& inside) const {
	const float3 v = transformation.position - ray.origin;
	const float b = math::dot(v, ray.direction);
	const float radius = transformation.scale[0];
	const float det = (b * b) - math::dot(v, v) + (radius * radius);

	if (det > 0.f) {
		const float dist = std::sqrt(det);
		const float t0 = b - dist;

		if (t0 > ray.min_t && t0 < ray.max_t) {
			ray.max_t = t0;
			epsilon = 5e-4f * t0;
			inside = false;
			return true;
		}

		const float t1 = b + dist;

		if (t1 > ray.min_t && t1 < ray.max_t) {
			ray.max_t = t1;
			epsilon = 5e-4f * t1;
			inside = true;
			return true;
		}
	}

	return false;
}

bool Sphere::intersect_p(const Transformation& transformation, const Ray& ray,
						 Node_stack& /*node_stack*/) const {
	float3 v = transformation.position - ray.origin;
	float b = math::dot(v, ray.direction);
	float radius = transformation.scale[0];
	float det = (b * b) - math::dot(v, v) + (radius * radius);

	if (det > 0.f) {
		float dist = std::sqrt(det);
		float t0 = b - dist;

		if (t0 > ray.min_t && t0 < ray.max_t) {
			return true;
		}

		float t1 = b + dist;

		if (t1 > ray.min_t && t1 < ray.max_t) {
			return true;
		}
	}

	return false;
}

float Sphere::opacity(const Transformation& transformation, const Ray& ray,
					  const Materials& materials,
					  Sampler_filter filter, const Worker& worker) const {
	float3 v = transformation.position - ray.origin;
	float b = math::dot(v, ray.direction);
	float radius = transformation.scale[0];
	float det = (b * b) - math::dot(v, v) + (radius * radius);

	if (det > 0.f) {
		float dist = std::sqrt(det);
		float t0 = b - dist;

		if (t0 > ray.min_t && t0 < ray.max_t) {
			float3 n = math::normalize(ray.point(t0) - transformation.position);
			float3 xyz = math::transform_vector_transposed(n, transformation.rotation);
			xyz = math::normalize(xyz);
			float2 uv = float2(-std::atan2(xyz[0], xyz[2]) * (math::Pi_inv * 0.5f) + 0.5f,
								std::acos(xyz[1]) * math::Pi_inv);

			return materials[0]->opacity(uv, ray.time, filter, worker);
		}

		float t1 = b + dist;

		if (t1 > ray.min_t && t1 < ray.max_t) {
			float3 n = math::normalize(ray.point(t1) - transformation.position);
			float3 xyz = math::transform_vector_transposed(n, transformation.rotation);
			xyz = math::normalize(xyz);
			float2 uv = float2(-std::atan2(xyz[0], xyz[2]) * (math::Pi_inv * 0.5f) + 0.5f,
								std::acos(xyz[1]) * math::Pi_inv);

			return materials[0]->opacity(uv, ray.time, filter, worker);
		}
	}

	return 0.f;
}

float3 Sphere::thin_absorption(const Transformation& transformation, const Ray& ray,
							   const Materials& materials,
							   Sampler_filter filter, const Worker& worker) const {
	float3 v = transformation.position - ray.origin;
	float b = math::dot(v, ray.direction);
	float radius = transformation.scale[0];
	float det = (b * b) - math::dot(v, v) + (radius * radius);

	if (det > 0.f) {
		float dist = std::sqrt(det);
		float t0 = b - dist;

		if (t0 > ray.min_t && t0 < ray.max_t) {
			float3 n = math::normalize(ray.point(t0) - transformation.position);
			float3 xyz = math::transform_vector_transposed(n, transformation.rotation);
			xyz = math::normalize(xyz);
			float2 uv = float2(-std::atan2(xyz[0], xyz[2]) * (math::Pi_inv * 0.5f) + 0.5f,
								std::acos(xyz[1]) * math::Pi_inv);

			return materials[0]->thin_absorption(ray.direction, n, uv,
												 ray.time, filter, worker);
		}

		float t1 = b + dist;

		if (t1 > ray.min_t && t1 < ray.max_t) {
			float3 n = math::normalize(ray.point(t1) - transformation.position);
			float3 xyz = math::transform_vector_transposed(n, transformation.rotation);
			xyz = math::normalize(xyz);
			float2 uv = float2(-std::atan2(xyz[0], xyz[2]) * (math::Pi_inv * 0.5f) + 0.5f,
								std::acos(xyz[1]) * math::Pi_inv);

			return materials[0]->thin_absorption(ray.direction, n, uv,
												 ray.time, filter, worker);
		}
	}

	return float3(0.f);
}

bool Sphere::sample(uint32_t part, const Transformation& transformation,
					const float3& p, const float3& /*n*/, float area, bool two_sided,
					sampler::Sampler& sampler, uint32_t sampler_dimension,
					Node_stack& node_stack, Sample& sample) const {
	return Sphere::sample(part, transformation, p, area, two_sided,
						  sampler, sampler_dimension, node_stack, sample);
}

bool Sphere::sample(uint32_t /*part*/, const Transformation& transformation,
					const float3& p, float /*area*/, bool /*two_sided*/,
					sampler::Sampler& sampler, uint32_t sampler_dimension,
					Node_stack& /*node_stack*/, Sample& sample) const {
	const float3 axis = transformation.position - p;
	const float axis_squared_length = math::squared_length(axis);
	const float radius = transformation.scale[0];
	const float radius_square = radius * radius;
	const float sin_theta_max2 = radius_square / axis_squared_length;
	float cos_theta_max  = std::sqrt(std::max(0.f, 1.f - sin_theta_max2));
	cos_theta_max = std::min(0.99999995f, cos_theta_max);
	const float pdf = math::cone_pdf_uniform(cos_theta_max);

	const float axis_length = std::sqrt(axis_squared_length);
	const float3 z = axis / axis_length;
	float3 x, y;
	math::orthonormal_basis(z, x, y);

	const float2 r2 = sampler.generate_sample_2D(sampler_dimension);
	const float3 dir = math::sample_oriented_cone_uniform(r2, cos_theta_max, x, y, z);

	const float b = math::dot(axis, dir);
	const float det = (b * b) - axis_squared_length + radius_square;
	const float dist = std::sqrt(det);
	const float t = b - dist;

	sample.wi = dir;
	sample.pdf = pdf;
	sample.t = t;
	sample.epsilon = 5e-4f * t;

	return true;
}

float Sphere::pdf(const Ray& ray, const shape::Intersection& /*intersection*/,
				  const Transformation& transformation,
				  float /*area*/, bool /*two_sided*/, bool /*total_sphere*/) const {
	const float3 axis = transformation.position - ray.origin;
	const float axis_squared_length = math::squared_length(axis);
	const float radius_square = transformation.scale[0] * transformation.scale[0];
	const float sin_theta_max2 = radius_square / axis_squared_length;
	float cos_theta_max  = std::sqrt(std::max(0.f, 1.f - sin_theta_max2));
	cos_theta_max = std::min(0.99999995f, cos_theta_max);

	return math::cone_pdf_uniform(cos_theta_max);
}

bool Sphere::sample(uint32_t /*part*/, const Transformation& transformation, const float3& p,
					float2 uv, float area, bool /*two_sided*/, Sample& sample) const {
	float phi   = (uv[0] + 0.75f) * (2.f * math::Pi);
	float theta = uv[1] * math::Pi;

	float sin_theta = std::sin(theta);
	float cos_theta = std::cos(theta);
	float sin_phi   = std::sin(phi);
	float cos_phi   = std::cos(phi);

	float3 ls(sin_theta * cos_phi, cos_theta, sin_theta * sin_phi);
	float3 ws = math::transform_point(ls, transformation.object_to_world);

	float3 axis = ws - p;
	float sl = math::squared_length(axis);
	float d = std::sqrt(sl);

	float3 dir = axis / d;

	float3 wn = math::normalize(ws - transformation.position);

	float c = -math::dot(wn, dir);

	if (c <= 0.f) {
		return false;
	}

	sample.wi = dir;
	sample.uv = uv;
	sample.t  = d;
	// sin_theta because of the uv weight
	sample.pdf = sl / (c * area * sin_theta);

	return true;
}

float Sphere::pdf_uv(const Ray& ray, const Intersection& intersection,
					 const Transformation& /*transformation*/,
					 float area, bool /*two_sided*/) const {
//	float3 xyz = math::transform_vector_transposed(wn, transformation.rotation);
//	uv[0] = -std::atan2(xyz[0], xyz[2]) * (math::Pi_inv * 0.5f) + 0.5f;
//	uv[1] =  std::acos(xyz[1]) * math::Pi_inv;

//	// sin_theta because of the uv weight
//	float sin_theta = std::sqrt(1.f - xyz[1] * xyz[1]);

	const float sin_theta = std::sin(intersection.uv[1] * math::Pi);

	const float sl = ray.max_t * ray.max_t;
	const float c = -math::dot(intersection.geo_n, ray.direction);
	return sl / (c * area * sin_theta);
}

float Sphere::uv_weight(float2 uv) const {
	const float sin_theta = std::sin(uv[1] * math::Pi);

	if (0.f == sin_theta) {
		// this case never seemed to be an issue?!
		return 0.f;
	}

	return 1.f / sin_theta;
}

float Sphere::area(uint32_t /*part*/, const float3& scale) const {
	return (4.f * math::Pi) * (scale[0] * scale[0]);
}

size_t Sphere::num_bytes() const {
	return sizeof(*this);
}

}
