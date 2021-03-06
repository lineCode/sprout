#include "volumetric_height.hpp"
#include "base/math/aabb.inl"
#include "base/math/exp.hpp"
#include "base/math/ray.inl"
#include "scene/entity/composed_transformation.hpp"
#include "scene/material/material_sample_helper.hpp"

#include "base/debug/assert.hpp"

namespace scene::material::volumetric {

/*
Height::Height(Sampler_settings const& sampler_settings) : Density(sampler_settings) {}

float3 Height::optical_depth(Transformation const& transformation, math::AABB const& aabb,
                                                         math::Ray const& ray, float step_size,
rnd::Generator& rng, Filter filter, Worker const& worker) const { const math::Ray rn =
ray.normalized();

        // This is an optimization of the generic stochastic method
        // implemented in Density::opptical_depth.
        // Because everything happens in world space there could be differences
        // when the volume is rotated because the local aabb is never checked.

        float const ay = rn.origin[1] + rn.min_t * rn.direction[1];
        float const by = rn.origin[1] + rn.max_t * rn.direction[1];

        float const min_y = aabb.min()[1];
        float const ha = ay - min_y;
        float const hb = by - min_y;

        float3 const attenuation = absorption_coefficient_ + scattering_coefficient_;

        float const d = rn.max_t - rn.min_t;

        float const hb_ha = hb - ha;

        if (0.f == hb_ha) {
                // special case where density stays exactly the same along the ray
                float3 const result = d * (a_ * math::exp(-b_ * ha)) * attenuation;

                SOFT_ASSERT(math::all_finite(result));

                return result;
        }

        // calculate the integral of the exponential density function
//	float fa = -((a_ * std::exp(-b_ * ha)) / b_);
//	float fb = -((a_ * std::exp(-b_ * hb)) / b_);

//	float3 result = d * ((fb - fa) / (hb - ha)) * attenuation;

        float const fa = -math::exp(-b_ * ha);
        float const fb = -math::exp(-b_ * hb);

        float3 const result = d * ((a_ * (fb - fa) / b_) / (hb_ha)) * attenuation;

        SOFT_ASSERT(math::all_finite(result));

        return result;

//	float3 old_result = Density::optical_depth(ray, step_size, rng, filter, worker);
//	return old_result;
}


void Height::set_a_b(float a, float b) {
        a_ = a;
        b_ = b;
}

size_t Height::num_bytes() const {
        return sizeof(*this);
}

float Height::density(float3 const& p, Transformation const& transformation,
                                          Filter filter, Worker const& worker) const {
        // p is in object space already

        // calculate scaled height
        float const height = transformation.scale[1] * (1.f + p[1]);

        return a_ * math::exp(-b_ * height);
}

*/
}
