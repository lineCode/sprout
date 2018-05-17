#pragma once

#include "substitute_base_material.hpp"

namespace scene::material::substitute {

class Material_translucent : public Material_base {

public:

	Material_translucent(Sampler_settings const& sampler_settings);

	virtual const material::Sample& sample(f_float3 wo, Renderstate const& rs,
										   Sampler_filter filter, sampler::Sampler& sampler,
										   Worker const& worker) const override final;

	virtual size_t num_bytes() const override final;

	void set_thickness(float thickness);
	void set_attenuation_distance(float attenuation_distance);

	static size_t sample_size();

private:

	float thickness_;
	float attenuation_distance_;
};

}
