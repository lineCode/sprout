#pragma once

#include "scene/material/material.hpp"

namespace scene::material::metal {

class Material_isotropic : public Material {

public:

	Material_isotropic(const Sampler_settings& sampler_settings, bool two_sided);

	virtual const material::Sample& sample(f_float3 wo, const Renderstate& rs,
										   Sampler_filter filter, sampler::Sampler& sampler,
										   const Worker& worker) const override final;

	virtual float ior() const override final;

	virtual size_t num_bytes() const override final;

	void set_normal_map(const Texture_adapter& normal_map);

	void set_ior(const float3& ior);
	void set_absorption(const float3& absorption);
	void set_roughness(float roughness);

	static size_t sample_size();

protected:

	Texture_adapter normal_map_;

	float3 ior_;
	float3 absorption_;

	float roughness_;
};

class Material_anisotropic : public Material {

public:

	Material_anisotropic(const Sampler_settings& sampler_settings, bool two_sided);

	virtual const material::Sample& sample(f_float3 wo, const Renderstate& rs,
										   Sampler_filter filter, sampler::Sampler& sampler,
										   const Worker& worker) const override final;

	virtual float ior() const override final;

	virtual size_t num_bytes() const override final;

	void set_normal_map(const Texture_adapter& normal_map);
	void set_direction_map(const Texture_adapter& direction_map);

	void set_ior(const float3& ior);
	void set_absorption(const float3& absorption);
	void set_roughness(float2 roughness);

	static size_t sample_size();

protected:

	Texture_adapter normal_map_;
	Texture_adapter direction_map_;

	float3 ior_;
	float3 absorption_;

	float2 roughness_;
};

}
