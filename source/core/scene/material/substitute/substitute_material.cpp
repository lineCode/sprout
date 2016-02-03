#include "substitute_material.hpp"
#include "substitute_sample.hpp"
#include "image/texture/sampler/sampler_2d.hpp"
#include "scene/material/material_sample.inl"
#include "scene/material/material_sample_cache.inl"
#include "scene/shape/geometry/differential.inl"
#include "base/math/vector.inl"

namespace scene { namespace material { namespace substitute {

Material::Material(Generic_sample_cache<Sample>& cache,
				   std::shared_ptr<image::texture::Texture_2D> mask, bool two_sided) :
	material::Material<Generic_sample_cache<Sample>>(cache, mask, two_sided) {}

const material::Sample& Material::sample(const shape::Differential& dg, const math::float3& wo,
										 float /*time*/, float /*ior_i*/,
										 const image::texture::sampler::Sampler_2D& sampler,
										 uint32_t worker_id) {
	auto& sample = cache_.get(worker_id);

	if (normal_map_) {
		math::float3 nm = sampler.sample_3(*normal_map_, dg.uv);
		math::float3 n = math::normalized(dg.tangent_to_world(nm));

		sample.set_basis(dg.t, dg.b, n, dg.geo_n, wo, two_sided_);
	} else {
		sample.set_basis(dg.t, dg.b, dg.n, dg.geo_n, wo, two_sided_);
	}

	math::float3 color;

	if (color_map_) {
		color = sampler.sample_3(*color_map_, dg.uv);
	} else {
		color = color_;
	}

	math::float2 surface;

	if (surface_map_) {
		surface  = sampler.sample_2(*surface_map_, dg.uv);
	} else {
		surface.x = roughness_;
		surface.y = metallic_;
	}

	float thickness;

	thickness = thickness_;

	if (emission_map_) {
		math::float3 emission = emission_factor_ * sampler.sample_3(*emission_map_, dg.uv);
		sample.set(color, emission, surface.x, surface.y, thickness, attenuation_distance_);
	} else {
		sample.set(color, math::float3::identity, surface.x, surface.y, thickness, attenuation_distance_);
	}

	return sample;
}

math::float3 Material::sample_emission(math::float2 uv, const image::texture::sampler::Sampler_2D& sampler) const {
	if (emission_map_) {
		return emission_factor_ * sampler.sample_3(*emission_map_, uv);
	} else {
		return math::float3::identity;
	}
}

math::float3 Material::average_emission() const {
	if (emission_map_) {
		return emission_factor_ * emission_map_->average().xyz();
	} else {
		return math::float3::identity;
	}
}

const image::texture::Texture_2D* Material::emission_map() const {
	return emission_map_.get();
}

void Material::set_color_map(std::shared_ptr<image::texture::Texture_2D> color_map) {
	color_map_ = color_map;
}

void Material::set_normal_map(std::shared_ptr<image::texture::Texture_2D> normal_map) {
	normal_map_ = normal_map;
}

void Material::set_surface_map(std::shared_ptr<image::texture::Texture_2D> surface_map) {
	surface_map_ = surface_map;
}

void Material::set_emission_map(std::shared_ptr<image::texture::Texture_2D> emission_map) {
	emission_map_ = emission_map;
}

void Material::set_color(const math::float3& color) {
	color_ = color;
}

void Material::set_roughness(float roughness) {
	roughness_ = roughness;
}

void Material::set_metallic(float metallic) {
	metallic_ = metallic;
}

void Material::set_emission_factor(float emission_factor) {
	emission_factor_ = emission_factor;
}

void Material::set_thickness(float thickness) {
	thickness_ = thickness;
}

void Material::set_attenuation_distance(float attenuation_distance) {
	attenuation_distance_ = attenuation_distance;
}

}}}
