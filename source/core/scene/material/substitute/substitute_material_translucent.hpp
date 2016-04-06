#pragma once

#include "substitute_material_base.hpp"

namespace scene { namespace material { namespace substitute {

class Sample_translucent;

class Material_translucent : public Material_base<Sample_translucent> {
public:

	Material_translucent(Generic_sample_cache<Sample_translucent>& cache,
						 std::shared_ptr<image::texture::Texture_2D> mask,
						 const Sampler_settings& sampler_settings, bool two_sided);

	virtual const material::Sample& sample(const shape::Differential& dg, const math::float3& wo,
										   float time, float ior_i,
										   const Worker& worker, Sampler_settings::Filter filter) final override;

	void set_thickness(float thickness);
	void set_attenuation_distance(float attenuation_distance);

private:

	float thickness_;
	float attenuation_distance_;
};

}}}