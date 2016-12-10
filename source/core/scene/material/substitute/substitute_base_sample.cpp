#include "substitute_base_sample.hpp"
#include "rendering/integrator/surface/integrator_helper.hpp"
#include "scene/material/material_sample.inl"
#include "scene/material/fresnel/fresnel.inl"
#include "scene/material/disney/disney.inl"
#include "scene/material/ggx/ggx.inl"
#include "sampler/sampler.hpp"
#include "base/math/vector.inl"
#include "base/math/math.hpp"
#include "base/math/sampling/sampling.inl"

namespace scene { namespace material { namespace substitute {

const material::Sample::Layer& Sample_base::base_layer() const {
	return layer_;
}

float3 Sample_base::radiance() const {
	return layer_.emission_;
}

float3 Sample_base::attenuation() const {
	return float3(100.f, 100.f, 100.f);
}

float Sample_base::ior() const {
	return layer_.ior_;
}

bool Sample_base::is_pure_emissive() const {
	return false;
}

bool Sample_base::is_transmissive() const {
	return false;
}

bool Sample_base::is_translucent() const {
	return false;
}

void Sample_base::Layer::set(float3_p color, float3_p radiance, float ior,
							 float constant_f0, float roughness, float metallic) {
	diffuse_color_ = (1.f - metallic) * color;
	f0_ = math::lerp(float3(constant_f0), color, metallic);
	emission_ = radiance;
	ior_ = ior;
	roughness_ = roughness;
	a2_ = math::pow4(roughness);
	metallic_ = metallic;
}

float3 Sample_base::Layer::base_evaluate(float3_p wi, float3_p wo, float& pdf) const {
	float n_dot_wi = clamped_n_dot(wi);
	float n_dot_wo = clamped_n_dot(wo);

	float3 h = math::normalized(wo + wi);
	float h_dot_wi = math::saturate(math::dot(h, wi));

	float d_pdf;
	float3 d_reflection = disney::Isotropic::reflection(h_dot_wi, n_dot_wi, n_dot_wo, *this, d_pdf);

	fresnel::Schlick schlick(f0_);
	float3 ggx_fresnel;
	float  ggx_pdf;
	float3 ggx_reflection = ggx::Isotropic::reflection(wi, wo, n_dot_wi, n_dot_wo, *this,
													   schlick, ggx_fresnel, ggx_pdf);

	pdf = 0.5f * (d_pdf + ggx_pdf);

	return n_dot_wi * ((1.f - ggx_fresnel) * d_reflection + ggx_reflection);
}

void Sample_base::Layer::diffuse_sample(float3_p wo, sampler::Sampler& sampler,
										bxdf::Result& result) const {
	float n_dot_wo = clamped_n_dot(wo);
	float n_dot_wi = disney::Isotropic::reflect(wo, n_dot_wo, *this, sampler, result);

	fresnel::Schlick schlick(f0_);
	float3 ggx_fresnel;
	float  ggx_pdf;
	float3 ggx_reflection = ggx::Isotropic::reflection(result.wi, wo, n_dot_wi, n_dot_wo,
													   *this, schlick, ggx_fresnel, ggx_pdf);

	result.reflection = n_dot_wi * ((1.f - ggx_fresnel) * result.reflection + ggx_reflection);
	result.pdf = 0.5f * (result.pdf + ggx_pdf);
}

void Sample_base::Layer::specular_sample(float3_p wo, sampler::Sampler& sampler,
										 bxdf::Result& result) const {
	float n_dot_wo = clamped_n_dot(wo);

	fresnel::Schlick schlick(f0_);
	float3 ggx_fresnel;
	float n_dot_wi = ggx::Isotropic::reflect(wo, n_dot_wo, *this, schlick,
											 sampler, ggx_fresnel, result);

	float d_pdf;
	float3 d_reflection = disney::Isotropic::reflection(result.h_dot_wi, n_dot_wi,
														n_dot_wo, *this, d_pdf);

	result.reflection = n_dot_wi * (result.reflection + (1.f - ggx_fresnel) * d_reflection);
	result.pdf = 0.5f * (result.pdf + d_pdf);
}

void Sample_base::Layer::pure_specular_sample(float3_p wo, sampler::Sampler& sampler,
											  bxdf::Result& result) const {
	float n_dot_wo = clamped_n_dot(wo);

	fresnel::Schlick schlick(f0_);
	float n_dot_wi = ggx::Isotropic::reflect(wo, n_dot_wo, *this, schlick, sampler, result);
	result.reflection *= n_dot_wi;
}

}}}
