#pragma once

#include "scene/material/material.hpp"
#include "scene/material/material_sample_cache.hpp"
#include "image/texture/texture_2d.hpp"
#include "base/math/distribution/distribution_2d.hpp"

namespace scene { namespace material { namespace display {

class Sample;

class Material : public material::Material<Generic_sample_cache<Sample>> {
public:

	Material(Generic_sample_cache<Sample>& cache, std::shared_ptr<image::texture::Texture_2D> mask, bool two_sided);

	virtual const material::Sample& sample(const shape::Differential& dg, const math::float3& wo,
										   float time, float ior_i,
										   const image::texture::sampler::Sampler_2D& sampler,
										   uint32_t worker_id) final override;

	virtual math::float3 sample_emission(math::float2 uv, float time,
										 const image::texture::sampler::Sampler_2D& sampler) const final override;

	virtual math::float3 average_emission() const final override;

	virtual bool has_emission_map() const final override;

	virtual math::float2 emission_importance_sample(math::float2 r2, float& pdf) const final override;

	virtual float emission_pdf(math::float2 uv,
							   const image::texture::sampler::Sampler_2D& sampler) const final override;

	virtual void prepare_sampling(bool spherical) final override;

	void set_emission_map(std::shared_ptr<image::texture::Texture_2D> emission_map);

	void set_emission(const math::float3& emission);
	void set_emission_factor(float emission_factor);
	void set_roughness(float roughness);
	void set_ior(float ior);

private:

	std::shared_ptr<image::texture::Texture_2D> emission_map_;

	math::float3 emission_;

	float emission_factor_;

	float roughness_;

	float f0_;

	math::float3 average_emission_;

	float total_weight_;

	math::Distribution_2D distribution_;
};

}}}
