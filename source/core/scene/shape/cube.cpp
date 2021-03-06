#include "cube.hpp"
#include "base/math/aabb.inl"
#include "base/math/matrix3x3.inl"
#include "base/math/sampling.inl"
#include "base/math/vector3.inl"
#include "sampler/sampler.hpp"
#include "scene/entity/composed_transformation.inl"
#include "scene/scene_ray.inl"
#include "scene/scene_worker.hpp"
#include "shape_intersection.hpp"
#include "shape_sample.hpp"

#include "base/debug/assert.hpp"
#include "shape_test.hpp"

namespace scene::shape {

Cube::Cube() noexcept {
    aabb_.set_min_max(float3(-1.f), float3(1.f));
    inv_extent_ = 1.f / aabb_.extent();
}

bool Cube::intersect(Ray& ray, Transformation const& transformation, Node_stack& node_stack,
                     Intersection& intersection) const noexcept {
    bool const hit = intersect_fast(ray, transformation, node_stack, intersection);

    intersection.n = intersection.geo_n;

    return hit;
}

bool Cube::intersect_fast(Ray& ray, Transformation const&           transformation,
                          Node_stack& /*node_stack*/, Intersection& intersection) const noexcept {
    float3 const local_origin = transformation.world_to_object_point(ray.origin);
    float3 const local_dir    = transformation.world_to_object_vector(ray.direction);

    math::Ray const local_ray(local_origin, local_dir, ray.min_t, ray.max_t);

    math::AABB const aabb(float3(-1.f), float3(1.f));

    float hit_t;
    if (!aabb.intersect_p(local_ray, hit_t)) {
        return false;
    }

    if (hit_t > ray.max_t) {
        return false;
    }

    ray.max_t = hit_t;

    intersection.p = ray.point(hit_t);

    float3 const local_p = local_ray.point(hit_t);

    float3 const distance = math::abs(1.f - math::abs(local_p));

    uint32_t const i = math::index_min_component(distance);

    float3 normal(0.f);
    normal[i] = math::copysign1(local_p[i]);

    intersection.geo_n = math::transform_vector(transformation.rotation, normal);

    intersection.epsilon = 3e-3f * hit_t;
    intersection.part    = 0;

    return true;
}

bool Cube::intersect(Ray& ray, Transformation const& transformation, Node_stack& /*node_stack*/,
                     float& epsilon) const noexcept {
    float3 v      = transformation.position - ray.origin;
    float  b      = math::dot(v, ray.direction);
    float  radius = transformation.scale[0];
    float  det    = (b * b) - math::dot(v, v) + (radius * radius);

    if (det > 0.f) {
        float dist = std::sqrt(det);
        float t0   = b - dist;

        if (t0 > ray.min_t && t0 < ray.max_t) {
            ray.max_t = t0;
            epsilon   = 5e-4f * t0;
            return true;
        }

        float t1 = b + dist;

        if (t1 > ray.min_t && t1 < ray.max_t) {
            ray.max_t = t1;
            epsilon   = 5e-4f * t1;
            return true;
        }
    }

    return false;
}

bool Cube::intersect_p(Ray const& ray, Transformation const& transformation,
                       Node_stack& /*node_stack*/) const noexcept {
    float3 const local_origin = transformation.world_to_object_point(ray.origin);
    float3 const local_dir    = transformation.world_to_object_vector(ray.direction);

    math::Ray const local_ray(local_origin, local_dir, ray.min_t, ray.max_t);

    math::AABB const aabb(float3(-1.f), float3(1.f));

    float hit_t;
    if (!aabb.intersect_p(local_ray, hit_t)) {
        return false;
    }

    if (hit_t > ray.max_t) {
        return false;
    }

    return true;
}

float Cube::opacity(Ray const& ray, Transformation const& transformation,
                    Materials const& materials, Filter filter, Worker const& worker) const
    noexcept {
    float3 v      = transformation.position - ray.origin;
    float  b      = math::dot(v, ray.direction);
    float  radius = transformation.scale[0];
    float  det    = (b * b) - math::dot(v, v) + (radius * radius);

    if (det > 0.f) {
        float dist = std::sqrt(det);
        float t0   = b - dist;

        if (t0 > ray.min_t && t0 < ray.max_t) {
            float3 n = math::normalize(ray.point(t0) - transformation.position);

            float3 xyz = math::transform_vector_transposed(transformation.rotation, n);
            xyz        = math::normalize(xyz);

            float2 uv = float2(-std::atan2(xyz[0], xyz[2]) * (math::Pi_inv * 0.5f) + 0.5f,
                               std::acos(xyz[1]) * math::Pi_inv);

            return materials[0]->opacity(uv, ray.time, filter, worker);
        }

        float t1 = b + dist;

        if (t1 > ray.min_t && t1 < ray.max_t) {
            float3 n = math::normalize(ray.point(t1) - transformation.position);

            float3 xyz = math::transform_vector_transposed(transformation.rotation, n);
            xyz        = math::normalize(xyz);

            float2 uv = float2(-std::atan2(xyz[0], xyz[2]) * (math::Pi_inv * 0.5f) + 0.5f,
                               std::acos(xyz[1]) * math::Pi_inv);

            return materials[0]->opacity(uv, ray.time, filter, worker);
        }
    }

    return 0.f;
}

float3 Cube::thin_absorption(Ray const& /*ray*/, Transformation const& /*transformation*/,
                             Materials const& /*materials*/, Filter /*filter*/,
                             Worker const& /*worker*/) const noexcept {
    return float3(0.f);
}

bool Cube::sample(uint32_t part, float3 const& p, float3 const& /*n*/,
                  Transformation const& transformation, float area, bool two_sided,
                  sampler::Sampler& sampler, uint32_t sampler_dimension, Node_stack& node_stack,
                  Sample_to& sample) const noexcept {
    return Cube::sample(part, p, transformation, area, two_sided, sampler, sampler_dimension,
                        node_stack, sample);
}

bool Cube::sample(uint32_t /*part*/, float3 const& p, Transformation const& transformation,
                  float /*area*/, bool /*two_sided*/, sampler::Sampler&     sampler,
                  uint32_t sampler_dimension, Node_stack& /*node_stack*/, Sample_to& sample) const
    noexcept {
    float3 const axis                = transformation.position - p;
    float const  axis_squared_length = math::squared_length(axis);
    float const  radius              = transformation.scale[0];
    float const  radius_square       = radius * radius;
    float const  sin_theta_max2      = radius_square / axis_squared_length;
    float        cos_theta_max       = std::sqrt(std::max(0.f, 1.f - sin_theta_max2));
    cos_theta_max                    = std::min(0.99999995f, cos_theta_max);

    float const  axis_length = std::sqrt(axis_squared_length);
    float3 const z           = axis / axis_length;

    auto const [x, y] = math::orthonormal_basis(z);

    float2 const r2  = sampler.generate_sample_2D(sampler_dimension);
    float3 const dir = math::sample_oriented_cone_uniform(r2, cos_theta_max, x, y, z);

    sample.wi = dir;

    sample.pdf     = math::cone_pdf_uniform(cos_theta_max);
    float const d  = axis_length - radius;  // this is not accurate
    sample.t       = d;
    sample.epsilon = 5e-4f * d;

    return true;
}

bool Cube::sample(uint32_t /*part*/, Transformation const& /*transformation*/, float /*area*/,
                  bool /*two_sided*/, sampler::Sampler& /*sampler*/, uint32_t /*sampler_dimension*/,
                  math::AABB const& /*bounds*/, Node_stack& /*node_stack*/,
                  Sample_from& /*sample*/) const noexcept {
    return false;
}

float Cube::pdf(Ray const&            ray, const shape::Intersection& /*intersection*/,
                Transformation const& transformation, float /*area*/, bool /*two_sided*/,
                bool /*total_sphere*/) const noexcept {
    float3 const axis                = transformation.position - ray.origin;
    float const  axis_squared_length = math::squared_length(axis);
    float const  radius_square       = transformation.scale[0] * transformation.scale[0];
    float const  sin_theta_max2      = radius_square / axis_squared_length;
    float        cos_theta_max       = std::sqrt(std::max(0.f, 1.f - sin_theta_max2));
    cos_theta_max                    = std::min(0.99999995f, cos_theta_max);

    return math::cone_pdf_uniform(cos_theta_max);
}

bool Cube::sample(uint32_t /*part*/, float3 const& /*p*/, float2 /*uv*/,
                  Transformation const& /*transformation*/, float /*area*/, bool /*two_sided*/,
                  Sample_to& /*sample*/) const noexcept {
    return false;
}

bool Cube::sample(uint32_t /*part*/, float2 /*uv*/, Transformation const& /*transformation*/,
                  float /*area*/, bool /*two_sided*/, sampler::Sampler& /*sampler*/,
                  uint32_t /*sampler_dimension*/, math::AABB const& /*bounds*/,
                  Sample_from& /*sample*/) const noexcept {
    return false;
}

float Cube::pdf_uv(Ray const& ray, Intersection const&             intersection,
                   Transformation const& /*transformation*/, float area, bool /*two_sided*/) const
    noexcept {
    //	float3 xyz = math::transform_vector_transposed(wn, transformation.rotation);
    //	uv[0] = -std::atan2(xyz[0], xyz[2]) * (math::Pi_inv * 0.5f) + 0.5f;
    //	uv[1] =  std::acos(xyz[1]) * math::Pi_inv;

    //	// sin_theta because of the uv weight
    //	float sin_theta = std::sqrt(1.f - xyz[1] * xyz[1]);

    float const sin_theta = std::sin(intersection.uv[1] * math::Pi);

    float const sl = ray.max_t * ray.max_t;
    float const c  = -math::dot(intersection.geo_n, ray.direction);
    return sl / (c * area * sin_theta);
}

float Cube::uv_weight(float2 uv) const noexcept {
    float const sin_theta = std::sin(uv[1] * math::Pi);

    if (0.f == sin_theta) {
        // this case never seemed to be an issue?!
        return 0.f;
    }

    return 1.f / sin_theta;
}

float Cube::area(uint32_t /*part*/, float3 const& scale) const noexcept {
    return (4.f * math::Pi) * (scale[0] * scale[0]);
}

size_t Cube::num_bytes() const noexcept {
    return sizeof(*this);
}

}  // namespace scene::shape
