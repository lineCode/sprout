#include "mix_material.hpp"
#include "sampler/sampler.hpp"
#include "scene/scene_renderstate.hpp"
#include "scene/scene_worker.inl"
#include "image/texture/texture_adapter.inl"

namespace scene::material::mix {

Material::Material(const Sampler_settings& sampler_settings, bool two_sided) :
	material::Material(sampler_settings, two_sided) {}

const material::Sample& Material::sample(const float3& wo, const Renderstate& rs,
                                         Sampler_filter filter, sampler::Sampler& sampler,
                                         const Worker& worker) const {
	auto& texture_sampler = worker.sampler_2D(sampler_key(), filter);
	const float mask = mask_.sample_1(texture_sampler, rs.uv);

	if (mask > sampler.generate_sample_1D(1)) {
		return material_a_->sample(wo, rs, filter, sampler, worker);
	} else {
		return material_b_->sample(wo, rs, filter, sampler, worker);
	}
}

float Material::opacity(float2 /*uv*/, float /*time*/, Sampler_filter /*filter*/,
						const Worker& /*worker*/) const {
	return 1.f;
}

bool Material::is_masked() const {
	return material_a_->is_masked() || material_b_->is_masked();
}

size_t Material::num_bytes() const {
	return sizeof(*this);
}

void Material::set_materials(const Material_ptr& a, const Material_ptr& b) {
	material_a_ = a;
	material_b_ = b;
}

}