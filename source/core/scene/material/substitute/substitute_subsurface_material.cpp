#include "substitute_subsurface_material.hpp"
#include "substitute_subsurface_sample.hpp"
#include "substitute_base_sample.inl"
#include "substitute_base_material.inl"
#include "scene/scene_renderstate.hpp"
#include "scene/scene_worker.hpp"
#include "scene/material/bssrdf.hpp"

namespace scene { namespace material { namespace substitute {

Material_subsurface::Material_subsurface(Sample_cache& sample_cache,
										 const Sampler_settings& sampler_settings) :
	Material_base(sample_cache, sampler_settings, false) {}

const material::Sample& Material_subsurface::sample(float3_p wo, const Renderstate& rs,
													const Worker& worker, Sampler_filter filter) {
	auto& sample = sample_cache_.get<Sample_subsurface>(worker.id());

	auto& sampler = worker.sampler_2D(sampler_key(), filter);

	set_sample(wo, rs, sampler, sample);

	return sample;
}

const BSSRDF& Material_subsurface::bssrdf(const Worker& worker) {
	auto& bssrdf = sample_cache_.bssrdf(worker.id());

	bssrdf.set(absorption_, scattering_);

	return bssrdf;
}

bool Material_subsurface::is_subsurface() const {
	return true;
}

size_t Material_subsurface::num_bytes() const {
	return sizeof(*this);
}

void Material_subsurface::set_absorption(float3_p absorption) {
	absorption_ = absorption;
}

void Material_subsurface::set_scattering(float3_p scattering) {
	scattering_ = scattering;
}

}}}
