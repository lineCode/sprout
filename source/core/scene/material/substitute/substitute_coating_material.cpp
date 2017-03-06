#include "substitute_coating_material.inl"
#include "substitute_coating_sample.inl"
#include "scene/scene_renderstate.inl"
#include "scene/scene_worker.hpp"
#include "scene/material/coating/coating.inl"
#include "base/math/vector4.inl"

namespace scene { namespace material { namespace substitute {

Material_clearcoat::Material_clearcoat(Sample_cache& sample_cache,
									   const Sampler_settings& sampler_settings,
									   bool two_sided) :
	Material_coating<coating::Clearcoat>(sample_cache, sampler_settings, two_sided) {}

const material::Sample& Material_clearcoat::sample(float3_p wo, const Renderstate& rs,
												   const Worker& worker, Sampler_filter filter) {
	auto& sample = sample_cache_.get<Sample_clearcoat>(worker.id());

	auto& sampler = worker.sampler_2D(sampler_key(), filter);

	set_sample(wo, rs, sampler, sample);

	set_coating_basis(rs, sampler, sample);

	sample.coating_.set(coating_.f0_, coating_.a2_);

	return sample;
}

void Material_clearcoat::set_clearcoat(float ior, float roughness) {
	coating_.f0_ = fresnel::schlick_f0(1.f, ior);
	coating_.a2_ = math::pow4(ggx::clamp_roughness(roughness));
}

Material_thinfilm::Material_thinfilm(Sample_cache& sample_cache,
									 const Sampler_settings& sampler_settings,
									 bool two_sided) :
	Material_coating<coating::Thinfilm>(sample_cache, sampler_settings, two_sided) {}

const material::Sample& Material_thinfilm::sample(float3_p wo, const Renderstate& rs,
												  const Worker& worker, Sampler_filter filter) {
	auto& sample = sample_cache_.get<Sample_thinfilm>(worker.id());

	auto& sampler = worker.sampler_2D(sampler_key(), filter);

	set_sample(wo, rs, sampler, sample);

	set_coating_basis(rs, sampler, sample);

	sample.coating_.set(coating_.ior_, coating_.a2_, coating_.thickness_);

	return sample;
}

void Material_thinfilm::set_thinfilm(float ior, float roughness, float thickness) {
	coating_.ior_ = ior;
	coating_.a2_  = math::pow4(ggx::clamp_roughness(roughness));
	coating_.thickness_ = thickness;
}

}}}
