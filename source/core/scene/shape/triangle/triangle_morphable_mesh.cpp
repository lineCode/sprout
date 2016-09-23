#include "triangle_morphable_mesh.hpp"
#include "triangle_intersection.hpp"
#include "triangle_morph_target_collection.hpp"
#include "triangle_primitive_mt.hpp"
#include "bvh/triangle_bvh_builder_suh.inl"
#include "bvh/triangle_bvh_data_interleaved.inl"
#include "bvh/triangle_bvh_tree.inl"
#include "scene/scene_ray.inl"
#include "scene/entity/composed_transformation.hpp"
#include "scene/shape/shape_sample.hpp"
#include "scene/shape/geometry/shape_intersection.hpp"
#include "sampler/sampler.hpp"
#include "base/math/vector.inl"
#include "base/math/matrix.inl"
#include "base/math/distribution/distribution_1d.inl"

namespace scene { namespace shape { namespace triangle {

Morphable_mesh::Morphable_mesh(std::shared_ptr<Morph_target_collection> collection,
							   uint32_t num_parts) :
	collection_(collection), num_parts_(num_parts) {
	vertices_.resize(collection_->vertices(0).size());
}

void Morphable_mesh::init() {
	aabb_ = tree_.aabb();
}

uint32_t Morphable_mesh::num_parts() const {
	return num_parts_;
}

bool Morphable_mesh::intersect(const Transformation& transformation, Ray& ray,
							   Node_stack& node_stack, shape::Intersection& intersection) const {
	math::Oray tray;
	tray.origin = math::transform_point(ray.origin, transformation.world_to_object);
	tray.set_direction(math::transform_vector(ray.direction, transformation.world_to_object));
	tray.min_t = ray.min_t;
	tray.max_t = ray.max_t;

	Intersection pi;
	if (tree_.intersect(tray, node_stack, pi)) {
		intersection.epsilon = 3e-3f * tray.max_t;

		intersection.p = ray.point(tray.max_t);

		float3 n;
		float3 t;
		float2 uv;
		tree_.interpolate_triangle_data(pi.index, pi.uv, n, t, uv);

		intersection.geo_n = math::transform_vector(tree_.triangle_normal(pi.index),
													transformation.rotation);
		intersection.n = math::transform_vector(n, transformation.rotation);
		intersection.t = math::transform_vector(t, transformation.rotation);

	//	math::transform_vectors(transformation.rotation, n, t, intersection.n, intersection.t);

		float bitangent_sign = tree_.triangle_bitangent_sign(pi.index);
		intersection.b = bitangent_sign * math::cross(intersection.n, intersection.t);
		intersection.uv = uv;
		intersection.part = tree_.triangle_material_index(pi.index);

		ray.max_t = tray.max_t;
		return true;
	}

	return false;
}

bool Morphable_mesh::intersect_p(const Transformation& transformation,
								 const Ray& ray, Node_stack& node_stack) const {
	math::Oray tray;
	tray.origin = math::transform_point(ray.origin, transformation.world_to_object);
	tray.set_direction(math::transform_vector(ray.direction, transformation.world_to_object));
	tray.min_t = ray.min_t;
	tray.max_t = ray.max_t;

	return tree_.intersect_p(tray, node_stack);
}

float Morphable_mesh::opacity(const Transformation& transformation, const Ray& ray,
							  const material::Materials& materials,
							  Worker& worker, Sampler_filter filter) const {
	math::Oray tray;
	tray.origin = math::transform_point(ray.origin, transformation.world_to_object);
	tray.set_direction(math::transform_vector(ray.direction, transformation.world_to_object));
	tray.min_t = ray.min_t;
	tray.max_t = ray.max_t;

	return tree_.opacity(tray, ray.time, materials, worker, filter);
}

void Morphable_mesh::sample(uint32_t /*part*/, const Transformation& /*transformation*/,
							float3_p /*p*/, float3_p /*n*/, float /*area*/,
							bool /*two_sided*/, sampler::Sampler& /*sampler*/,
							Node_stack& /*node_stack*/, Sample& /*sample*/) const {}

void Morphable_mesh::sample(uint32_t /*part*/, const Transformation& /*transformation*/,
							float3_p /*p*/, float /*area*/, bool /*two_sided*/,
							sampler::Sampler& /*sampler*/, Node_stack& /*node_stack*/,
							Sample& /*sample*/) const {}

void Morphable_mesh::sample(uint32_t /*part*/, const Transformation& /*transformation*/,
							float3_p /*p*/, float2 /*uv*/, float /*area*/,
							Sample& /*sample*/) const {}

void Morphable_mesh::sample(uint32_t /*part*/, const Transformation& /*transformation*/,
							float3_p /*p*/, float3_p /*wi*/, float /*area*/,
							Sample& /*sample*/) const {}

float Morphable_mesh::pdf(uint32_t /*part*/, const Transformation& /*transformation*/,
						  float3_p /*p*/, float3_p /*wi*/, float /*area*/,
						  bool /*two_sided*/, bool /*total_sphere*/,
						  shape::Node_stack& /*node_stack*/) const {
	return 1.f;
}

float Morphable_mesh::area(uint32_t /*part*/, float3_p /*scale*/) const {
	return 1.f;
}

float Morphable_mesh::uv_weight(float2 /*uv*/) const {
	return 1.f;
}

bool Morphable_mesh::is_complex() const {
	return true;
}

bool Morphable_mesh::is_analytical() const {
	return false;
}

void Morphable_mesh::prepare_sampling(uint32_t /*part*/) {}

Morphable_shape* Morphable_mesh::morphable_shape() {
	return this;
}

void Morphable_mesh::morph(uint32_t a, uint32_t b, float weight, thread::Pool& pool) {
	collection_->morph(a, b, weight, pool, vertices_);

	bvh::Builder_SUH builder;
	builder.build(tree_, collection_->triangles(), vertices_, 0, 8);

	init();
}

size_t Morphable_mesh::num_bytes() const {
	return sizeof(*this);
}

}}}


