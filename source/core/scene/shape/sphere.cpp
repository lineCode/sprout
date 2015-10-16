#include "sphere.hpp"
#include "shape_sample.hpp"
#include "geometry/shape_intersection.hpp"
#include "scene/entity/composed_transformation.hpp"
#include "sampler/sampler.hpp"
#include "base/math/sampling/sampling.inl"
#include "base/math/vector.inl"
#include "base/math/ray.inl"
#include "base/math/bounding/aabb.inl"

namespace scene { namespace shape {

Sphere::Sphere() {
	aabb_.set_min_max(math::float3(-1.f, -1.f, -1.f), math::float3(1.f, 1.f, 1.f));
}

bool Sphere::intersect(const entity::Composed_transformation& transformation, math::Oray& ray,
					   const math::float2& /*bounds*/, Node_stack& /*node_stack*/,
					   Intersection& intersection) const {
	math::float3 v = ray.origin - transformation.position;
	float b = -dot(v, ray.direction);
	float radius = transformation.scale.x;
	float det = (b * b) - dot(v, v) + (radius * radius);

	if (det > 0.f) {
		float dist = std::sqrt(det);
		float t0 = b - dist;

		if (t0 > ray.min_t && t0 < ray.max_t) {
			intersection.epsilon = 5e-4f * t0;

			intersection.p = ray.point(t0);
			intersection.n = math::normalized(intersection.p - transformation.position);
			math::coordinate_system(intersection.n, intersection.t, intersection.b);
			intersection.geo_n = intersection.n;

			math::float3 xyz = math::transform_vector_transposed(transformation.rotation, intersection.n);
			intersection.uv = math::float2((std::atan2(xyz.x, xyz.z) * math::Pi_inv) * 0.5f,
										   std::acos(xyz.y) * math::Pi_inv);

			intersection.part = 0;

			ray.max_t = t0;
			return true;
		}

		float t1 = b + dist;

		if (t1 > ray.min_t && t1 < ray.max_t) {
			intersection.epsilon = 5e-4f * t1;

			intersection.p = ray.point(t1);
			intersection.n = math::normalized(intersection.p - transformation.position);
			math::coordinate_system(intersection.n, intersection.t, intersection.b);
			intersection.geo_n = intersection.n;

			math::float3 xyz = math::transform_vector_transposed(transformation.rotation, intersection.n);
			intersection.uv = math::float2((std::atan2(xyz.x, xyz.z) * math::Pi_inv) * 0.5f,
										   std::acos(xyz.y) * math::Pi_inv);

			intersection.part = 0;

			ray.max_t = t1;
			return true;
		}
	}

	return false;
}

bool Sphere::intersect_p(const entity::Composed_transformation& transformation, const math::Oray& ray,
						 const math::float2& /*bounds*/, Node_stack& /*node_stack*/) const {
	math::float3 v = ray.origin - transformation.position;
	float b = -dot(v, ray.direction);
	float radius = transformation.scale.x;
	float det = (b * b) - dot(v, v) + (radius * radius);

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

float Sphere::opacity(const entity::Composed_transformation& transformation, const math::Oray& ray,
					  const math::float2& /*bounds*/, Node_stack& /*node_stack*/,
					  const material::Materials& materials,
					  const image::texture::sampler::Sampler_2D& sampler) const {
	math::float3 v = ray.origin - transformation.position;
	float b = -dot(v, ray.direction);
	float radius = transformation.scale.x;
	float det = (b * b) - dot(v, v) + (radius * radius);

	if (det > 0.f) {
		float dist = std::sqrt(det);
		float t0 = b - dist;

		if (t0 > ray.min_t && t0 < ray.max_t) {
			math::float3 n = math::normalized(ray.point(t0) - transformation.position);
			math::float3 xyz = math::transform_vector_transposed(transformation.rotation, n);
			math::float2 uv = math::float2((std::atan2(xyz.x, xyz.z) * math::Pi_inv) * 0.5f,
										   std::acos(xyz.y) * math::Pi_inv);

			return materials[0]->opacity(uv, sampler);
		}

		float t1 = b + dist;

		if (t1 > ray.min_t && t1 < ray.max_t) {
			math::float3 n = math::normalized(ray.point(t1) - transformation.position);
			math::float3 xyz = math::transform_vector_transposed(transformation.rotation, n);
			math::float2 uv = math::float2((std::atan2(xyz.x, xyz.z) * math::Pi_inv) * 0.5f,
										   std::acos(xyz.y) * math::Pi_inv);

			return materials[0]->opacity(uv, sampler);
		}
	}

	return 0.f;
}

void Sphere::sample(uint32_t /*part*/, const entity::Composed_transformation& transformation, float /*area*/,
					const math::float3& p, const math::float3& /*n*/, bool /*restrict_to_hemisphere*/,
					sampler::Sampler& sampler, Sample& sample) const {
	math::float3 axis = transformation.position - p;
	float axis_squared_length = math::squared_length(axis);

	float radius_square = transformation.scale.x * transformation.scale.x;
	float sin_theta_max2 = radius_square / axis_squared_length;
	float cos_theta_max  = std::sqrt(std::max(0.f, 1.f - sin_theta_max2));
	cos_theta_max = std::min(0.99999995f, cos_theta_max);

	float axis_length = std::sqrt(axis_squared_length);
	math::float3 z = axis / axis_length;
	math::float3 x, y;
	math::coordinate_system(z, x, y);

	math::float2 r2 = sampler.generate_sample_2D();
	math::float3 dir = math::sample_oriented_cone_uniform(r2, cos_theta_max, x, y, z);

	sample.wi = dir;
	sample.t = axis_length - transformation.scale.x; // this is not accurate
	sample.pdf = math::cone_pdf_uniform(cos_theta_max);

//	if (std::isinf(sample.pdf)) {
//		sample.pdf = 1.f;
//	}
}

void Sphere::sample(uint32_t /*part*/, const entity::Composed_transformation& /*transformation*/, float /*area*/,
					const math::float3& /*p*/, math::float2 /*uv*/, Sample& /*sample*/) const {}

void Sphere::sample(uint32_t /*part*/, const entity::Composed_transformation& /*transformation*/, float /*area*/,
					const math::float3& /*p*/, const math::float3& /*wi*/, Sample& /*sample*/) const {}

float Sphere::pdf(uint32_t /*part*/, const entity::Composed_transformation& transformation, float /*area*/,
				  const math::float3& p, const math::float3& /*wi*/, bool /*restrict_to_hemisphere*/) const {
	math::float3 axis = transformation.position - p;
	float axis_squared_length = math::squared_length(axis);

	float radius_square = transformation.scale.x * transformation.scale.x;
	float sin_theta_max2 = radius_square / axis_squared_length;
	float cos_theta_max  = std::sqrt(std::max(0.f, 1.f - sin_theta_max2));
	cos_theta_max = std::min(0.99999995f, cos_theta_max);

	return math::cone_pdf_uniform(cos_theta_max);
}

float Sphere::area(uint32_t /*part*/, const math::float3& scale) const {
	return 4.f * math::Pi * scale.x * scale.x;
}

}}
