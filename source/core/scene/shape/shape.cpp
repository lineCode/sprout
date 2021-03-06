#include "shape.hpp"
#include "base/math/aabb.inl"
#include "base/math/matrix3x3.inl"
#include "base/math/vector3.inl"

namespace scene::shape {

Shape::~Shape() noexcept {}

math::AABB const& Shape::aabb() const noexcept {
    return aabb_;
}

float3 Shape::object_to_texture_point(float3 const& p) const noexcept {
    return (p - aabb_.bounds[0]) * inv_extent_;
}

float3 Shape::object_to_texture_vector(float3 const& v) const noexcept {
    return v * inv_extent_;
}

math::AABB Shape::transformed_aabb(float4x4 const& m, math::Transformation const& /*t*/) const
    noexcept {
    return aabb_.transform(m);
}

math::AABB Shape::transformed_aabb(math::Transformation const& t) const noexcept {
    return transformed_aabb(float4x4(t), t);
}

uint32_t Shape::num_parts() const noexcept {
    return 1;
}

// bool Shape::intersect_p(FVector ray_origin, FVector ray_direction,
//						FVector ray_min_t, FVector ray_max_t,
//						Transformation const& transformation,
//						Node_stack& node_stack) const {
//	return false;
//}

bool Shape::is_complex() const noexcept {
    return false;
}

bool Shape::is_finite() const noexcept {
    return true;
}

bool Shape::is_analytical() const noexcept {
    return true;
}

void Shape::prepare_sampling(uint32_t /*part*/) noexcept {}

Morphable_shape* Shape::morphable_shape() noexcept {
    return nullptr;
}

}  // namespace scene::shape
