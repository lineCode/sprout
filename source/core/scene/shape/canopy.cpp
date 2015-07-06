#include "canopy.hpp"
#include "shape_sample.hpp"
#include "geometry/shape_intersection.hpp"
#include "scene/entity/composed_transformation.hpp"
#include "sampler/sampler.hpp"
#include "base/math/sampling.hpp"
#include "base/math/vector.inl"
#include "base/math/matrix.inl"
#include "base/math/ray.inl"
#include "base/math/bounding/aabb.inl"
#include "base/math/print.hpp"
#include <iostream>

namespace scene { namespace shape {

Canopy::Canopy() {
	aabb_.set_min_max(math::float3::identity, math::float3::identity);
}

bool Canopy::intersect(const entity::Composed_transformation& transformation, math::Oray& ray,
					   const math::float2& /*bounds*/, Node_stack& /*node_stack*/,
					   Intersection& intersection) const {
	if (ray.max_t >= 1000.f) {
		intersection.epsilon = 5e-4f;

		intersection.p = ray.point(1000.f);
		intersection.t = transformation.rotation.x;
		intersection.b = transformation.rotation.y;
		intersection.n = -ray.direction;
		intersection.geo_n = intersection.n;
		intersection.part = 0;

		math::float3 xyz = math::transform_vector_transposed(transformation.rotation, ray.direction);

		intersection.uv = math::float2((std::atan2(xyz.x, xyz.z) * math::Pi_inv ) * 0.5f, std::acos(xyz.y) * math::Pi_inv);

//		if (intersection.uv.x < 0.f) {
//			intersection.uv.x += 1.f;
//		}

		ray.max_t = 1000.f;
		return true;
	}

	return false;
}

bool Canopy::intersect_p(const entity::Composed_transformation& /*transformation*/, const math::Oray& /*ray*/,
						 const math::float2& /*bounds*/, Node_stack& /*node_stack*/) const {
	// Implementation for this is not really needed, so just skip it
	return false;
}

float Canopy::opacity(const entity::Composed_transformation& /*transformation*/, const math::Oray& /*ray*/,
					  const math::float2& /*bounds*/, Node_stack& /*node_stack*/,
					  const material::Materials& /*materials*/, const image::sampler::Sampler_2D& /*sampler*/) const {
	// Implementation for this is not really needed, so just skip it
	return 0.f;
}

void Canopy::sample(uint32_t /*part*/, const entity::Composed_transformation& transformation, float /*area*/,
					const math::float3& /*p*/, const math::float3& n,
					sampler::Sampler& sampler, Sample& sample) const {
	math::float3 x, y;
	math::coordinate_system(n, x, y);

	math::float2 uv = sampler.generate_sample_2D();
	math::float3 dir = math::sample_oriented_hemisphere_uniform(uv, x, y, n);

	sample.wi  = dir;

	math::float3 xyz = math::transform_vector_transposed(transformation.rotation, dir);
	sample.uv = math::float2((std::atan2(xyz.x, xyz.z) * math::Pi_inv + 0.5f) * 0.5f, std::acos(xyz.y) * math::Pi_inv);

	sample.t   = 1000.f;
	sample.pdf = 1.f / (2.f * math::Pi);
}

void Canopy::sample(uint32_t /*part*/, const entity::Composed_transformation& transformation, float /*area*/,
					const math::float3& /*p*/, const math::float2& uv, Sample& sample) const {

	math::float2 tuv = uv;
	tuv.x += 0.25f;
//	tuv.y -= 0.25f;
	tuv.x = math::frac(tuv.x);
//	tuv.y = math::frac(tuv.y);
//	math::float3 dir = math::sample_sphere_uniform(tuv);


//	float theta = uv.x * 2.f * math::Pi;
//	float phi   = uv.y * math::Pi;

	float phi   = tuv.y * 2.f * math::Pi;
	float theta = tuv.x * math::Pi;

	float sin_theta = std::sin(theta);
	float cos_theta = std::cos(theta);
	float sin_phi   = std::sin(phi);
	float cos_phi   = std::cos(phi);

	math::float3 dir = math::float3(sin_theta * cos_phi, sin_theta * sin_phi, cos_theta);
	//math::float3 dir = math::float3(sin_theta * sin_phi, sin_theta * cos_phi, cos_theta);

	sample.wi = math::transform_vector(transformation.rotation, math::normalized(dir));

	sample.wi.x *= -1.f;
//	sample.wi.z *= -1.f;w

	sample.uv = uv;

	sample.t  = 1000.f;
}

float Canopy::pdf(uint32_t /*part*/, const entity::Composed_transformation& /*transformation*/, float /*area*/,
				  const math::float3& /*p*/, const math::float3& /*wi*/) const {
	return 1.f / (2.f * math::Pi);
}

float Canopy::area(uint32_t /*part*/, const math::float3& /*scale*/) const {
	return 4.f * math::Pi;
}

bool Canopy::is_finite() const {
	return false;
}

}}

