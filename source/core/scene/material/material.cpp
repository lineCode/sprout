#include "material.hpp"
#include "image/texture/sampler/sampler_2d.hpp"

namespace scene { namespace material {

IMaterial::IMaterial(std::shared_ptr<image::texture::Texture_2D> mask) :
	mask_(mask) {}

math::float2 IMaterial::emission_importance_sample(math::float2 /*r2*/, float& /*pdf*/) const {
	return math::float2::identity;
}

float IMaterial::emission_pdf(math::float2 /*uv*/,
							  const image::texture::sampler::Sampler_2D& /*sampler*/) const {
	return 0.f;
}

void IMaterial::prepare_sampling(bool /*spherical*/) {}

bool IMaterial::is_masked() const {
	return !mask_ == false;
}

float IMaterial::opacity(math::float2 uv, const image::texture::sampler::Sampler_2D& sampler) const {
	if (mask_) {
		return sampler.sample_1(*mask_, uv);
	} else {
		return 1.f;
	}
}

}}
