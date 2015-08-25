#pragma once

#include "substitute_constant.hpp"
#include "scene/material/material_sample.inl"
#include "scene/material/material_sample_cache.inl"
#include "scene/shape/geometry/differential.hpp"
#include "base/math/vector.inl"

namespace scene { namespace material { namespace substitute {

template<bool Two_sided>
Constant<Two_sided>::Constant(Generic_sample_cache<Sample>& cache,
							  std::shared_ptr<image::texture::Texture_2D> mask,
							  const math::float3& color,
							  float roughness,
							  float metallic) :
	Substitute(cache, mask), color_(color), roughness_(roughness), metallic_(metallic) {}

template<bool Two_sided>
const Sample& Constant<Two_sided>::sample(const shape::Differential& dg, const math::float3& wo,
										  const image::texture::sampler::Sampler_2D& /*sampler*/,
										  uint32_t worker_id) {
	auto& sample = cache_.get(worker_id);

	sample.template set_basis<Two_sided>(dg.t, dg.b, dg.n, dg.geo_n, wo);
	sample.set(color_, roughness_, metallic_);

	return sample;
}

}}}
