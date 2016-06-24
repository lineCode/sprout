#include "substitute_translucent_material.hpp"
#include "substitute_translucent_sample.hpp"
#include "substitute_base_material.inl"
#include "scene/scene_renderstate.hpp"

namespace scene { namespace material { namespace substitute {

Material_translucent::Material_translucent(Generic_sample_cache<Sample_translucent>& cache,
										   Texture_2D_ptr mask,
										   const Sampler_settings& sampler_settings,
										   bool two_sided) :
	Material_base<Sample_translucent>(cache, mask, sampler_settings, two_sided) {}

const material::Sample& Material_translucent::sample(float3_p wo, const Renderstate& rs,
													 const Worker& worker,
													 Sampler_filter filter) {
	auto& sample = cache_.get(worker.id());

	auto& sampler = worker.sampler(sampler_key_, filter);

	float side = sample.set_basis(rs.geo_n, wo);

	if (normal_map_) {
		float3 nm = sampler.sample_3(*normal_map_, rs.uv);
		float3 n = math::normalized(rs.tangent_to_world(nm));

		sample.layer_.set_basis(rs.t, rs.b, n, side);
	} else {
		sample.layer_.set_basis(rs.t, rs.b, rs.n, side);
	}

	float3 color;
	if (color_map_) {
		color = sampler.sample_3(*color_map_, rs.uv);
	} else {
		color = color_;
	}

	float2 surface;
	if (surface_map_) {
		surface = sampler.sample_2(*surface_map_, rs.uv);
		surface.x = math::pow4(surface.x);
	} else {
		surface.x = a2_;
		surface.y = metallic_;
	}

	if (emission_map_) {
		float3 radiance = emission_factor_ * sampler.sample_3(*emission_map_, rs.uv);
		sample.layer_.set(color, radiance, ior_, constant_f0_, surface.x, surface.y);
	} else {
		sample.layer_.set(color, math::float3_identity, ior_, constant_f0_, surface.x, surface.y);
	}

	float thickness;

	thickness = thickness_;
	sample.set(color, surface.y, thickness, attenuation_distance_);

	return sample;
}

void Material_translucent::set_thickness(float thickness) {
	thickness_ = thickness;
}

void Material_translucent::set_attenuation_distance(float attenuation_distance) {
	attenuation_distance_ = attenuation_distance;
}

}}}
