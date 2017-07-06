#include "triangle_mesh.hpp"
#include "triangle_intersection.hpp"
// #include "bvh/triangle_bvh_data.inl"
#include "bvh/triangle_bvh_indexed_data.inl"
// #include "bvh/triangle_bvh_data_interleaved.inl"
#include "bvh/triangle_bvh_tree.inl"
#include "scene/scene_constants.hpp"
#include "scene/scene_ray.inl"
#include "scene/entity/composed_transformation.hpp"
#include "scene/shape/shape_intersection.hpp"
#include "scene/shape/shape_sample.hpp"
#include "sampler/sampler.hpp"
#include "base/math/vector3.inl"
#include "base/math/matrix.inl"
#include "base/math/matrix3x3.inl"
#include "base/math/distribution/distribution_1d.inl"

namespace scene { namespace shape { namespace triangle {

bool Mesh::init() {
	aabb_ = tree_.aabb();

	distributions_.resize(tree_.num_parts());

	return 0 != tree_.num_parts();
}

Mesh::Tree& Mesh::tree() {
	return tree_;
}

uint32_t Mesh::num_parts() const {
	return tree_.num_parts();
}

bool Mesh::intersect(const Transformation& transformation, Ray& ray,
					 Node_stack& node_stack, shape::Intersection& intersection) const {
/*	math::Ray tray;
	tray.origin = math::transform_point(ray.origin, transformation.world_to_object);
	tray.set_direction(math::transform_vector(ray.direction, transformation.world_to_object));
	tray.min_t = ray.min_t;
	tray.max_t = ray.max_t;

	Intersection pi;
	if (tree_.intersect(tray, node_stack, pi)) {
		ray.max_t = tray.max_t;

		float epsilon = 3e-3f * tray.max_t;

		float3 p_w = ray.point(tray.max_t);

		Vector n;
		Vector t;
		float2 uv;
		tree_.interpolate_triangle_data(pi.u, pi.v, pi.index, n, t, uv);

		Vector	 geo_n			= tree_.triangle_normal_v(pi.index);
		Vector	 bitangent_sign = simd::set_float4(tree_.triangle_bitangent_sign(pi.index));
		uint32_t material_index = tree_.triangle_material_index(pi.index);

		Matrix3 rotation = math::load_float3x3(transformation.rotation);

		Vector geo_n_w = math::transform_vector(rotation, geo_n);
		Vector n_w	   = math::transform_vector(rotation, n);
		Vector t_w	   = math::transform_vector(rotation, t);
		Vector b_w	   = math::mul(bitangent_sign, math::cross3(n_w, t_w));

		intersection.p = p_w;
		simd::store_float4(intersection.t.v, t_w);
		simd::store_float4(intersection.b.v, b_w);
		simd::store_float4(intersection.n.v, n_w);
		simd::store_float4(intersection.geo_n.v, geo_n_w);
		intersection.uv = uv;
		intersection.epsilon = epsilon;
		intersection.part = material_index;

		return true;
	}

	return false;*/

	Matrix4 world_to_object = math::load_float4x4(transformation.world_to_object);
	Vector ray_origin = simd::load_float4(ray.origin.v);
	ray_origin = math::transform_point(world_to_object, ray_origin);
	Vector ray_direction = simd::load_float4(ray.direction.v);
	ray_direction = math::transform_vector(world_to_object, ray_direction);

	Vector ray_inv_direction = math::reciprocal3(ray_direction);
	alignas(16) uint32_t ray_signs[4];
	math::sign(ray_inv_direction, ray_signs);

	Vector ray_min_t = simd::load_float(&ray.min_t);
	Vector ray_max_t = simd::load_float(&ray.max_t);

	Intersection pi;
	if (tree_.intersect(ray_origin, ray_direction, ray_inv_direction,
						ray_min_t, ray_max_t, ray_signs, node_stack, pi)) {
		float tray_max_t = simd::get_x(ray_max_t);
		ray.max_t = tray_max_t;

		float epsilon = 3e-3f * tray_max_t;

		float3 p_w = ray.point(tray_max_t);

		Vector n;
		Vector t;
		float2 uv;
		tree_.interpolate_triangle_data(pi.u, pi.v, pi.index, n, t, uv);

		Vector	 geo_n			= tree_.triangle_normal_v(pi.index);
		Vector	 bitangent_sign = simd::set_float4(tree_.triangle_bitangent_sign(pi.index));
		uint32_t material_index = tree_.triangle_material_index(pi.index);

		Matrix3 rotation = math::load_float3x3(transformation.rotation);

		Vector geo_n_w = math::transform_vector(rotation, geo_n);
		Vector n_w	   = math::transform_vector(rotation, n);
		Vector t_w	   = math::transform_vector(rotation, t);
		Vector b_w	   = math::mul(bitangent_sign, math::cross3(n_w, t_w));

		intersection.p = p_w;
		simd::store_float4(intersection.t.v, t_w);
		simd::store_float4(intersection.b.v, b_w);
		simd::store_float4(intersection.n.v, n_w);
		simd::store_float4(intersection.geo_n.v, geo_n_w);
		intersection.uv = uv;
		intersection.epsilon = epsilon;
		intersection.part = material_index;

		return true;
	}

	return false;
}

bool Mesh::intersect_p(const Transformation& transformation, const Ray& ray,
					   Node_stack& node_stack) const {
//	math::Ray tray;
//	tray.origin = math::transform_point(ray.origin, transformation.world_to_object);
//	tray.set_direction(math::transform_vector(ray.direction, transformation.world_to_object));
//	tray.min_t = ray.min_t;
//	tray.max_t = ray.max_t;

//	return tree_.intersect_p(tray, node_stack);

	Matrix4 world_to_object = math::load_float4x4(transformation.world_to_object);
	Vector ray_origin = simd::load_float4(ray.origin.v);
	ray_origin = math::transform_point(world_to_object, ray_origin);
	Vector ray_direction = simd::load_float4(ray.direction.v);
	ray_direction = math::transform_vector(world_to_object, ray_direction);

	Vector ray_inv_direction = math::reciprocal3(ray_direction);
	alignas(16) uint32_t ray_signs[4];
	math::sign(ray_inv_direction, ray_signs);

	Vector ray_min_t = simd::load_float(&ray.min_t);
	Vector ray_max_t = simd::load_float(&ray.max_t);

	return tree_.intersect_p(ray_origin, ray_direction, ray_inv_direction,
							 ray_min_t, ray_max_t, ray_signs, node_stack);
}

//bool Mesh::intersect_p(VVector ray_origin, VVector ray_direction,
//					   VVector ray_min_t, VVector ray_max_t,
//					   const Transformation& transformation,
//					   Node_stack& node_stack) const {
//	Matrix4 world_to_object = math::load_float4x4(transformation.world_to_object);
//	Vector tray_origin = math::transform_point(world_to_object, ray_origin);
//	Vector tray_direction = math::transform_vector(world_to_object, ray_direction);

//	Vector tray_inv_direction = math::reciprocal3(tray_direction);
//	alignas(16) uint32_t ray_signs[4];
//	math::sign(tray_inv_direction, ray_signs);

//	return tree_.intersect_p(tray_origin, tray_direction, tray_inv_direction,
//							 ray_min_t, ray_max_t, ray_signs, node_stack);
//}

float Mesh::opacity(const Transformation& transformation, const Ray& ray,
					const material::Materials& materials,
					Worker& worker, Sampler_filter filter) const {
	math::Ray tray;
	tray.origin = math::transform_point(ray.origin, transformation.world_to_object);
	tray.set_direction(math::transform_vector(ray.direction, transformation.world_to_object));
	tray.min_t = ray.min_t;
	tray.max_t = ray.max_t;

	return tree_.opacity(tray, ray.time, materials, worker, filter);
}

float3 Mesh::thin_absorption(const Transformation& transformation, const Ray& ray,
							 const material::Materials& materials,
							 Worker& worker, Sampler_filter filter) const {
	math::Ray tray;
	tray.origin = math::transform_point(ray.origin, transformation.world_to_object);
	tray.set_direction(math::transform_vector(ray.direction, transformation.world_to_object));
	tray.min_t = ray.min_t;
	tray.max_t = ray.max_t;

	return tree_.absorption(tray, ray.time, materials, worker, filter);
}

void Mesh::sample(uint32_t part, const Transformation& transformation,
				  const float3& p, const float3& /*n*/, float area, bool two_sided,
				  sampler::Sampler& sampler, uint32_t sampler_dimension,
				  Node_stack& node_stack, Sample& sample) const {
	Mesh::sample(part, transformation, p, area, two_sided,
				 sampler, sampler_dimension, node_stack, sample);
}

void Mesh::sample(uint32_t part, const Transformation& transformation,
				  const float3& p, float area, bool two_sided,
				  sampler::Sampler& sampler, uint32_t sampler_dimension,
				  Node_stack& /*node_stack*/, Sample& sample) const {
	const float  r  = sampler.generate_sample_1D(sampler_dimension);
	const float2 r2 = sampler.generate_sample_2D(sampler_dimension);

	uint32_t index = distributions_[part].sample(r);

	float3 sv;
	float2 tc;
	tree_.sample(index, r2, sv, tc);
	float3 v = math::transform_point(sv, transformation.object_to_world);

	float3 sn = tree_.triangle_normal(index);
	float3 wn = math::transform_vector(sn, transformation.rotation);

	float3 axis = v - p;
	float sl = math::squared_length(axis);
	float d  = std::sqrt(sl);
	float3 dir = axis / d;

	float c = -math::dot(wn, dir);

	if (two_sided) {
		c = std::abs(c);
	}

	if (c <= 0.f) {
		sample.pdf = 0.f;
	} else {
		sample.wi = dir;
		sample.uv = tc;
		sample.t = d;
		sample.pdf = sl / (c * area);
	}
}

float Mesh::pdf(uint32_t part, const Transformation& transformation,
				const float3& p, const float3& wi, float offset, float area, bool two_sided,
				bool /*total_sphere*/, Node_stack& node_stack) const {
	math::Ray ray;
	ray.origin = math::transform_point(p, transformation.world_to_object);
	ray.set_direction(math::transform_vector(wi, transformation.world_to_object));
	ray.min_t = offset;
	ray.max_t = scene::Ray_max_t;

	Intersection pi;
	if (tree_.intersect(ray, node_stack, pi)) {
		uint32_t shape_part = tree_.triangle_material_index(pi.index);
		if (part != shape_part) {
			return 0.f;
		}

		float3 sn = tree_.triangle_normal(pi.index);
		float3 wn = math::transform_vector(sn, transformation.rotation);

		float c = -math::dot(wn, wi);

		if (two_sided) {
			c = std::abs(c);
		}

		if (c <= 0.f) {
			return 0.f;
		}

		float sl = ray.max_t * ray.max_t;
		return sl / (c * area);
	}

	return 0.f;
}

void Mesh::sample(uint32_t /*part*/, const Transformation& /*transformation*/,
				  const float3& /*p*/, float2 /*uv*/, float /*area*/, bool /*two_sided*/,
				  Sample& /*sample*/) const {}

float Mesh::pdf_uv(uint32_t /*part*/, const Transformation& /*transformation*/,
				   const float3& /*p*/, const float3& /*wi*/, float /*area*/, bool /*two_sided*/,
				   float2& /*uv*/) const {
	return 1.f;
}

float Mesh::pdf_uv(const float3& wi, const shape::Intersection& intersection,
				   float hit_t, float area, bool two_sided) const {
	return 1.f;
}

float Mesh::uv_weight(float2 /*uv*/) const {
	return 1.f;
}

float Mesh::area(uint32_t part, const float3& scale) const {
	// HACK: This only really works for uniform scales!
	return distributions_[part].distribution.integral() * scale[0] * scale[1];
}

bool Mesh::is_complex() const {
	return true;
}

bool Mesh::is_analytical() const {
	return false;
}

void Mesh::prepare_sampling(uint32_t part) {
   if (distributions_[part].empty()) {
	   distributions_[part].init(part, tree_);
   }
}

size_t Mesh::num_bytes() const {
	size_t num_bytes = 0;

	for (auto& d : distributions_) {
		num_bytes += d.num_bytes();
	}

	return sizeof(*this) + tree_.num_bytes() + num_bytes;
}

void Mesh::Distribution::init(uint32_t part, const Tree& tree) {
	uint32_t num_triangles = tree.num_triangles(part);

	std::vector<float> areas(num_triangles);

	triangle_mapping.resize(num_triangles);

	for (uint32_t t = 0, mt = 0, len = tree.num_triangles(); t < len; ++t) {
        if (tree.triangle_material_index(t) == part) {
			areas[mt] = tree.triangle_area(t);
			triangle_mapping[mt] = t;
			++mt;
		}
	}

	distribution.init(areas.data(), num_triangles);
}

bool Mesh::Distribution::empty() const {
	return triangle_mapping.empty();
}

uint32_t Mesh::Distribution::sample(float r) const {
	return triangle_mapping[distribution.sample_discrete(r)];
}

size_t Mesh::Distribution::num_bytes() const {
	return sizeof(*this) + triangle_mapping.size() * sizeof(uint32_t) + distribution.num_bytes();
}


}}}
