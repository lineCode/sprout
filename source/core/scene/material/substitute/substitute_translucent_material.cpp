#include "substitute_translucent_material.hpp"
#include "substitute_translucent_sample.hpp"
#include "substitute_base_material.inl"
#include "substitute_base_sample.inl"
#include "image/texture/texture_adapter.inl"
#include "scene/scene_renderstate.hpp"
#include "scene/scene_worker.inl"
#include "base/math/vector4.inl"

namespace scene::material::substitute {

Material_translucent::Material_translucent(const Sampler_settings& sampler_settings) :
	Material_base(sampler_settings, true) {}

const material::Sample& Material_translucent::sample(f_float3 wo, const Renderstate& rs,
													 Sampler_filter filter,
													 sampler::Sampler& /*sampler*/,
													 const Worker& worker) const {
	auto& sample = worker.sample<Sample_translucent>();

	auto& sampler = worker.sampler_2D(sampler_key(), filter);

	set_sample(wo, rs, sampler, sample);

	float thickness;

	thickness = thickness_;
	sample.set(sample.layer_.diffuse_color_, thickness, attenuation_distance_);

	return sample;
}

size_t Material_translucent::num_bytes() const {
	return sizeof(*this);
}

void Material_translucent::set_thickness(float thickness) {
	thickness_ = thickness;
}

void Material_translucent::set_attenuation_distance(float attenuation_distance) {
	attenuation_distance_ = attenuation_distance;
}

size_t Material_translucent::sample_size() {
	return sizeof(Sample_translucent);
}

}
