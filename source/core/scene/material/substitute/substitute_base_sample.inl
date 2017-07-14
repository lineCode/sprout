#pragma once

#include "substitute_base_sample.hpp"
#include "scene/material/material_sample_helper.hpp"
#include "scene/material/coating/coating.inl"
#include "scene/material/disney/disney.inl"
#include "base/math/vector3.inl"

namespace scene { namespace material { namespace substitute {

template<typename Diffuse>
const material::Sample::Layer& Sample_base<Diffuse>::base_layer() const {
	return layer_;
}

template<typename Diffuse>
float3 Sample_base<Diffuse>::radiance() const {
	return layer_.emission_;
}

template<typename Diffuse>
float Sample_base<Diffuse>::ior() const {
	return layer_.ior_;
}

template<typename Diffuse>
template<typename Coating>
float3 Sample_base<Diffuse>::base_and_coating_evaluate(const float3& wi, const Coating& coating,
													   float& pdf) const {
	const float3 h = math::normalize(wo_ + wi);
	const float wo_dot_h = clamp_dot(wo_, h);

	float3 coating_attenuation;
	float  coating_pdf;
	const float3 coating_reflection = coating.evaluate(wi, wo_, h, wo_dot_h, layer_.ior_,
													   coating_attenuation, coating_pdf);

	float base_pdf;
	const float3 base_reflection = layer_.base_evaluate(wi, wo_, h, wo_dot_h, base_pdf);

	pdf = (coating_pdf + 2.f * base_pdf) / 3.f;
	return coating_reflection + coating_attenuation * base_reflection;
}

template<typename Diffuse>
template<typename Coating>
void Sample_base<Diffuse>::base_and_coating_sample(const Coating& coating,
												   sampler::Sampler& sampler,
												   bxdf::Result& result) const {
	const float p = sampler.generate_sample_1D();

	if (p < 0.5f) {
		float3 coating_attenuation;
		coating.sample(wo_, layer_.ior_, sampler, coating_attenuation, result);

		float base_pdf;
		const float3 base_reflection = layer_.base_evaluate(result.wi, wo_, result.h,
															result.h_dot_wi, base_pdf);

		result.pdf = (result.pdf + 2.f * base_pdf) / 3.f;
		result.reflection = result.reflection + coating_attenuation * base_reflection;
	} else {
		if (1.f == layer_.metallic_) {
			pure_specular_sample_and_coating(coating, sampler, result);
		} else {
			if (p < 0.75f) {
				diffuse_sample_and_coating(coating, sampler, result);
			} else {
				specular_sample_and_coating(coating, sampler, result);
			}
		}
	}
}

template<typename Diffuse>
template<typename Coating>
void Sample_base<Diffuse>::diffuse_sample_and_coating(const Coating& coating,
													  sampler::Sampler& sampler,
													  bxdf::Result& result) const {
	layer_.diffuse_sample(wo_, sampler, result);

	float3 coating_attenuation;
	float  coating_pdf;
	const float3 coating_reflection = coating.evaluate(result.wi, wo_, result.h,
													   result.h_dot_wi, layer_.ior_,
													   coating_attenuation, coating_pdf);

	result.pdf = (2.f * result.pdf + coating_pdf) / 3.f;
	result.reflection = coating_attenuation * result.reflection + coating_reflection;
}

template<typename Diffuse>
template<typename Coating>
void Sample_base<Diffuse>::specular_sample_and_coating(const Coating& coating,
													   sampler::Sampler& sampler,
													   bxdf::Result& result) const {
	layer_.specular_sample(wo_, sampler, result);

	float3 coating_attenuation;
	float  coating_pdf;
	const float3 coating_reflection = coating.evaluate(result.wi, wo_, result.h,
													   result.h_dot_wi, layer_.ior_,
													   coating_attenuation, coating_pdf);

	result.pdf = (2.f * result.pdf + coating_pdf) / 3.f;
	result.reflection = coating_attenuation * result.reflection + coating_reflection;
}

template<typename Diffuse>
template<typename Coating>
void Sample_base<Diffuse>::pure_specular_sample_and_coating(const Coating& coating,
															sampler::Sampler& sampler,
															bxdf::Result& result) const {
	layer_.pure_specular_sample(wo_, sampler, result);

	float3 coating_attenuation;
	float  coating_pdf;
	const float3 coating_reflection = coating.evaluate(result.wi, wo_, result.h,
													   result.h_dot_wi, layer_.ior_,
													   coating_attenuation, coating_pdf);

	result.pdf = 0.5f * (result.pdf + coating_pdf);
	result.reflection = coating_attenuation * result.reflection + coating_reflection;
}

template<typename Diffuse>
void Sample_base<Diffuse>::Layer::set(const float3& color, const float3& radiance, float ior,
									  float constant_f0, float roughness, float metallic) {
	diffuse_color_ = (1.f - metallic) * color;
	f0_ = math::lerp(float3(constant_f0), color, metallic);
	emission_ = radiance;
	ior_ = ior;
	roughness_ = roughness;
	a2_ = math::pow4(roughness);
	metallic_ = metallic;
}

template<typename Diffuse>
float3 Sample_base<Diffuse>::Layer::base_evaluate(const float3& wi, const float3& wo,
												  const float3& h, float wo_dot_h,
												  float& pdf) const {
	const float n_dot_wi = clamp_n_dot(wi);
	const float n_dot_wo = clamp_abs_n_dot(wo); //clamp_n_dot(wo);

	float d_pdf;
	const float3 d_reflection = Diffuse::reflection(wo_dot_h, n_dot_wi, n_dot_wo, *this, d_pdf);

	const float n_dot_h = math::saturate(math::dot(n_, h));

	const fresnel::Schlick schlick(f0_);
	float3 ggx_fresnel;
	float  ggx_pdf;
	const float3 ggx_reflection = ggx::Isotropic::reflection(n_dot_wi, n_dot_wo, wo_dot_h, n_dot_h,
															 *this, schlick, ggx_fresnel, ggx_pdf);

	pdf = 0.5f * (d_pdf + ggx_pdf);

	return n_dot_wi * (/*(1.f - ggx_fresnel) **/ d_reflection + ggx_reflection);
}

template<typename Diffuse>
void Sample_base<Diffuse>::Layer::diffuse_sample(const float3& wo, sampler::Sampler& sampler,
												 bxdf::Result& result) const {
	const float n_dot_wo = clamp_abs_n_dot(wo); //clamp_n_dot(wo);
	const float n_dot_wi = Diffuse::reflect(wo, n_dot_wo, *this, sampler, result);

	const float n_dot_h = math::saturate(math::dot(n_, result.h));

	const fresnel::Schlick schlick(f0_);
	float3 ggx_fresnel;
	float  ggx_pdf;
	const float3 ggx_reflection = ggx::Isotropic::reflection(n_dot_wi, n_dot_wo, result.h_dot_wi,
															 n_dot_h, *this, schlick,
															 ggx_fresnel, ggx_pdf);

	result.reflection = n_dot_wi * (/*(1.f - ggx_fresnel) **/ result.reflection + ggx_reflection);
	result.pdf = 0.5f * (result.pdf + ggx_pdf);
}

template<typename Diffuse>
void Sample_base<Diffuse>::Layer::specular_sample(const float3& wo, sampler::Sampler& sampler,
												  bxdf::Result& result) const {
	const float n_dot_wo = clamp_abs_n_dot(wo); //clamp_n_dot(wo);

	const fresnel::Schlick schlick(f0_);
	float3 ggx_fresnel;
	const float n_dot_wi = ggx::Isotropic::reflect(wo, n_dot_wo, *this, schlick,
												   sampler, ggx_fresnel, result);

	float d_pdf;
	const float3 d_reflection = Diffuse::reflection(result.h_dot_wi, n_dot_wi,
													n_dot_wo, *this, d_pdf);

	result.reflection = n_dot_wi * (result.reflection + /*(1.f - ggx_fresnel) **/ d_reflection);
	result.pdf = 0.5f * (result.pdf + d_pdf);
}

template<typename Diffuse>
void Sample_base<Diffuse>::Layer::pure_specular_sample(const float3& wo, sampler::Sampler& sampler,
													   bxdf::Result& result) const {
	const float n_dot_wo = clamp_abs_n_dot(wo); //clamp_n_dot(wo);

	const fresnel::Schlick schlick(f0_);
	const float n_dot_wi = ggx::Isotropic::reflect(wo, n_dot_wo, *this, schlick, sampler, result);
	result.reflection *= n_dot_wi;
}

}}}
