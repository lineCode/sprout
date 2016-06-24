#include "cloth_material.hpp"
#include "cloth_sample.hpp"
#include "image/texture/sampler/sampler_2d.hpp"
#include "scene/scene_worker.hpp"
#include "scene/material/material_sample.inl"
#include "scene/material/material_sample_cache.inl"
#include "scene/shape/geometry/hitpoint.inl"
#include "base/math/vector.inl"

namespace scene { namespace material { namespace cloth {

Material::Material(Generic_sample_cache<Sample>& cache,
				   Texture_2D_ptr mask,
				   const Sampler_settings& sampler_settings, bool two_sided) :
	material::Typed_material<Generic_sample_cache<Sample>>(cache, mask,
														   sampler_settings,
														   two_sided) {}

const material::Sample& Material::sample(const shape::Hitpoint& hp, float3_p wo,
										 float /*area*/, float /*time*/, float /*ior_i*/,
										 const Worker& worker, Sampler_filter filter) {
	auto& sample = cache_.get(worker.id());

	auto& sampler = worker.sampler(sampler_key_, filter);

	float side = sample.set_basis(hp.geo_n, wo, two_sided_);
	if (normal_map_) {
		float3 nm = sampler.sample_3(*normal_map_, hp.uv);
		float3 n = side * math::normalized(hp.tangent_to_world(nm));
		sample.layer_.set_basis(hp.t, hp.b, n, side);
	} else {
		sample.layer_.set_basis(hp.t, hp.b, hp.n, side);
	}

	float3 color;
	if (color_map_) {
		color = sampler.sample_3(*color_map_, hp.uv);
	} else {
		color = color_;
	}

	sample.layer_.set(color);

	return sample;
}

void Material::set_color_map(Texture_2D_ptr color_map) {
	color_map_ = color_map;
}

void Material::set_normal_map(Texture_2D_ptr normal_map) {
	normal_map_ = normal_map;
}

void Material::set_color(const float3& color) {
	color_ = color;
}

}}}
