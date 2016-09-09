#include "metal_material.hpp"
#include "metal_sample.hpp"
#include "image/texture/texture_2d_adapter.inl"
#include "scene/scene_renderstate.hpp"
#include "scene/scene_worker.hpp"
#include "scene/material/ggx/ggx.inl"
#include "scene/material/material_sample.inl"
#include "scene/material/material_sample_cache.inl"
#include "scene/shape/geometry/hitpoint.inl"
#include "base/math/vector.inl"

namespace scene { namespace material { namespace metal {

Material_isotropic::Material_isotropic(Generic_sample_cache<Sample_isotropic>& cache,
									   const Sampler_settings& sampler_settings, bool two_sided) :
	material::Typed_material<Generic_sample_cache<Sample_isotropic>>(cache,
																	 sampler_settings,
																	 two_sided) {}

const material::Sample& Material_isotropic::sample(float3_p wo, const Renderstate& rs,
												   const Worker& worker,
												   Sampler_settings::Filter filter) {
	auto& sample = cache_.get(worker.id());

	sample.set_basis(rs.geo_n, wo);

	if (normal_map_.is_valid()) {
		auto& sampler = worker.sampler(sampler_key_, filter);

		float3 nm = normal_map_.sample_3(sampler, rs.uv);
		float3 n  = math::normalized(rs.tangent_to_world(nm));
		sample.layer_.set_basis(rs.t, rs.b, n);
	} else {
		sample.layer_.set_basis(rs.t, rs.b, rs.n);
	}

	sample.layer_.set(ior_, absorption_, roughness_);

	return sample;
}

size_t Material_isotropic::num_bytes() const {
	return sizeof(*this);
}

void Material_isotropic::set_normal_map(const Adapter_2D& normal_map) {
	normal_map_ = normal_map;
}

void Material_isotropic::set_ior(float3_p ior) {
	ior_ = ior;
}

void Material_isotropic::set_absorption(float3_p absorption) {
	absorption_ = absorption;
}

void Material_isotropic::set_roughness(float roughness) {
	roughness_ = ggx::clamp_roughness(roughness);
}

Material_anisotropic::Material_anisotropic(Generic_sample_cache<Sample_anisotropic>& cache,
										   const Sampler_settings& sampler_settings,
										   bool two_sided) :
	material::Typed_material<Generic_sample_cache<Sample_anisotropic>>(cache,
																	   sampler_settings,
																	   two_sided) {}

const material::Sample& Material_anisotropic::sample(float3_p wo, const Renderstate& rs,
													 const Worker& worker,
													 Sampler_settings::Filter filter) {
	auto& sample = cache_.get(worker.id());

	auto& sampler = worker.sampler(sampler_key_, filter);

	sample.set_basis(rs.geo_n, wo);

	if (normal_map_.is_valid()) {
		float3 nm = normal_map_.sample_3(sampler, rs.uv);
		float3 n  = math::normalized(rs.tangent_to_world(nm));

		sample.layer_.set_basis(rs.t, rs.b, n);
	} else if (direction_map_.is_valid()) {
		float2 tm = direction_map_.sample_2(sampler, rs.uv);
		float3 t  = math::normalized(rs.tangent_to_world(tm));
		float3 b  = math::cross(rs.n, t);

		sample.layer_.set_basis(t, b, rs.n);
	} else {
		sample.layer_.set_basis(rs.t, rs.b, rs.n);
	}

	sample.layer_.set(ior_, absorption_, roughness_);

	return sample;
}

size_t Material_anisotropic::num_bytes() const {
	return sizeof(*this);
}

void Material_anisotropic::set_normal_map(const Adapter_2D& normal_map) {
	normal_map_ = normal_map;
}

void Material_anisotropic::set_direction_map(const Adapter_2D& direction_map) {
	direction_map_ = direction_map;
}

void Material_anisotropic::set_ior(float3_p ior) {
	ior_ = ior;
}

void Material_anisotropic::set_absorption(float3_p absorption) {
	absorption_ = absorption;
}

void Material_anisotropic::set_roughness(float2 roughness) {
	roughness_ = float2(ggx::clamp_roughness(roughness.x),
						ggx::clamp_roughness(roughness.y));
}

}}}
