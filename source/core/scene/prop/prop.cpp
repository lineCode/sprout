#include "prop.hpp"
#include "scene/entity/composed_transformation.hpp"
#include "scene/shape/shape.hpp"
#include "base/math/vector.inl"
#include "base/math/matrix.inl"

namespace scene {

Prop::~Prop() {}

void Prop::init(std::shared_ptr<shape::Shape> shape, const material::Materials& materials) {
	shape_ = shape;
	materials_ = materials;

	has_masked_material_ = false;
	for (auto m : materials_) {
		if (m->is_masked()) {
			has_masked_material_ = true;
			break;
		}
	}
}

bool Prop::intersect(math::Oray& ray, Node_stack& node_stack, shape::Intersection& intersection) const {
	Composed_transformation transformation;
	bool animated = transformation_at(ray.time, transformation);

	math::float2 bounds;

	if (shape_->is_complex()) {
		math::AABB aabb;
		if (animated) {
			shape_->aabb().transform(transformation_.object_to_world, aabb);
		} else {
			aabb = aabb_;
		}

		if (!aabb_.intersect_p(ray)) {
			return false;
		}
	}

	float hit_t;
	bool hit = shape_->intersect(transformation, ray, bounds, node_stack, intersection, hit_t);
	if (hit) {
		ray.max_t = hit_t;
		return true;
	}

	return false;
}

bool Prop::intersect_p(const math::Oray& ray, Node_stack& node_stack) const {
	Composed_transformation transformation;
	bool animated = transformation_at(ray.time, transformation);

	math::float2 bounds;

	if (shape_->is_complex()) {
		math::AABB aabb;
		if (animated) {
			shape_->aabb().transform(transformation_.object_to_world, aabb);
		} else {
			aabb = aabb_;
		}

		if (!aabb_.intersect_p(ray)) {
			return false;
		}
	}

	return shape_->intersect_p(transformation, ray, bounds, node_stack);
}

float Prop::opacity(const math::Oray& ray, Node_stack& node_stack, const image::sampler::Sampler_2D& sampler) const {
	if (!has_masked_material()) {
		return intersect_p(ray, node_stack) ? 1.f : 0.f;
	}

	Composed_transformation transformation;
	bool animated = transformation_at(ray.time, transformation);

	math::float2 bounds;

	if (shape_->is_complex()) {
		math::AABB aabb;
		if (animated) {
			shape_->aabb().transform(transformation_.object_to_world, aabb);
		} else {
			aabb = aabb_;
		}

		if (!aabb_.intersect_p(ray)) {
			return 0.f;
		}
	}

	return shape_->opacity(transformation, ray, bounds, node_stack, materials_, sampler);
}

const shape::Shape* Prop::shape() const {
	return shape_.get();
}

const math::AABB& Prop::aabb() const {
	return aabb_;
}

material::IMaterial* Prop::material(uint32_t index) const {
	return materials_[index].get();
}

bool Prop::has_masked_material() const {
	return has_masked_material_;
}

void Prop::on_set_transformation() {
	shape_->aabb().transform(transformation_.object_to_world, aabb_);
}

}
