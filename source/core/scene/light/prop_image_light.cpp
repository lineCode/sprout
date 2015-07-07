#include "prop_image_light.hpp"
#include "light_sample.hpp"
#include "image/texture/texture_2d.inl"
#include "sampler/sampler.hpp"
#include "scene/prop/prop.hpp"
#include "scene/shape/shape.hpp"
#include "scene/shape/shape_sample.hpp"
#include "scene/material/material.hpp"
#include "base/math/math.hpp"
#include "base/math/sampling.hpp"
#include "base/math/distribution_2d.inl"
#include "base/math/vector.inl"
#include "base/math/matrix.inl"
#include "base/math/bounding/aabb.inl"

namespace scene { namespace light {

void Prop_image_light::sample(const entity::Composed_transformation& transformation, const math::float3& p, const math::float3& n,
							  const image::sampler::Sampler_2D& image_sampler, sampler::Sampler& sampler,
							  uint32_t max_samples, std::vector<Sample>& samples) const {

	samples.clear();

	Sample light_sample;

	auto material = prop_->material(part_);

	for (uint32_t i = 0; i < max_samples; ++i) {
		float pdf;
		math::float2 uv = material->emission_importance_sample(sampler.generate_sample_2D(), pdf);

		prop_->shape()->sample(part_, transformation, area_, p, uv, light_sample.shape);

		light_sample.shape.pdf *= pdf;

		if (math::dot(light_sample.shape.wi, n) < 0.f) {
			light_sample.shape.pdf = 0.f;
		} else {
			light_sample.energy = material->sample_emission(light_sample.shape.uv, image_sampler);
		}

		samples.push_back(light_sample);
	}
}

float Prop_image_light::pdf(const entity::Composed_transformation& transformation, const math::float3& p, const math::float3& wi) const {
	return prop_->shape()->pdf(part_, transformation, area_, p, wi);
}

void Prop_image_light::prepare_sampling() {
	prop_->material(part_)->prepare_sampling();

	entity::Composed_transformation transformation;
	prop_->transformation_at(0.f, transformation);
	area_ = prop_->shape()->area(part_, transformation.scale);
}

}}

