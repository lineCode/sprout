#include "matte_sample.hpp"
#include "scene/material/material_sample.inl"
#include "scene/material/disney/disney.inl"
#include "scene/material/lambert/lambert.inl"
#include "scene/material/oren_nayar/oren_nayar.inl"
#include "base/math/vector.inl"

namespace scene { namespace material { namespace matte {

float3_p Sample::shading_normal() const {
	return layer_.n;
}

float3 Sample::tangent_to_world(float3_p v) const {
	return layer_.tangent_to_world(v);
}

float3 Sample::evaluate(float3_p wi, float& pdf) const {
	float n_dot_wi = layer_.clamped_n_dot(wi);

//	float3 brdf = lambert::Isotropic::evaluate(layer_.diffuse_color, n_dot_wi, layer_, pdf);

	float n_dot_wo = layer_.clamped_n_dot(wo_);

	float3 brdf = disney::Isotropic::evaluate(wi, wo_, n_dot_wi, n_dot_wo, layer_, pdf);

//	float3 brdf = oren_nayar::Isotropic::evaluate(wi, wo_, n_dot_wi, n_dot_wo, layer_, pdf);

	return n_dot_wi * brdf;
}

float3 Sample::radiance() const {
	return math::float3_identity;
}

float3 Sample::attenuation() const {
	return float3(100.f, 100.f, 100.f);
}

float Sample::ior() const {
	return 1.47f;
}

void Sample::sample_evaluate(sampler::Sampler& sampler, bxdf::Result& result) const {
//	float n_dot_wi = lambert::Isotropic::importance_sample(layer_.diffuse_color, layer_,
//														   sampler, result);

	float n_dot_wo = layer_.clamped_n_dot(wo_);

	float n_dot_wi = disney::Isotropic::importance_sample(wo_, n_dot_wo, layer_,
														  sampler, result);

	result.reflection *= n_dot_wi;
}

bool Sample::is_pure_emissive() const {
	return false;
}

bool Sample::is_transmissive() const {
	return false;
}

bool Sample::is_translucent() const {
	return false;
}

void Sample::Layer::set(float3_p color) {
	this->diffuse_color = color;
	this->roughness = 1.f;
	this->a2 = math::pow4(this->roughness);
}

}}}
