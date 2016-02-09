#include "ggx.hpp"
#include "scene/material/bxdf.hpp"
#include "scene/material/fresnel/fresnel.inl"
#include "sampler/sampler.hpp"
#include "base/math/math.hpp"

namespace scene { namespace material { namespace ggx {

template<typename Sample>
math::float3 Schlick_isotropic<Sample>::evaluate(const Sample& sample,
												 const math::float3& wi, float n_dot_wi, float n_dot_wo,
												 float &pdf) const {
	// Roughness zero will always have zero specular term (or worse NaN)
	if (0.f == sample.a2_) {
		pdf = 0.f;
		return math::float3::identity;
	}

	math::float3 h = math::normalized(sample.wo_ + wi);

	float n_dot_h  = math::saturate(math::dot(sample.n_, h));
	float wo_dot_h = math::clamp(math::dot(sample.wo_, h), 0.00001f, 1.f);

	float d = distribution_isotropic(n_dot_h, std::max(sample.a2_, 0.0000001f));
	float g = geometric_shadowing(n_dot_wi, n_dot_wo, sample.a2_);
	math::float3 f = fresnel::schlick(wo_dot_h, sample.f0_);

	math::float3 specular = d * g * f;

//	if (math::contains_negative(specular)) {
//		std::cout << "GGX<Sample>::evaluate()" << std::endl;
//	}

//	if (math::contains_inf(specular) || math::contains_nan(specular)) {
//		std::cout << "GGX<Sample>::evaluate()" << std::endl;
//	}

	pdf = d * n_dot_h / (4.f * wo_dot_h);
	return specular;
}

template<typename Sample>
float Schlick_isotropic<Sample>::importance_sample(const Sample& sample,
												   sampler::Sampler& sampler, float n_dot_wo,
												   bxdf::Result& result) const {
	math::float2 xi = sampler.generate_sample_2D();

	// For zero roughness we risk NaN if xi.y == 1: n_dot_h is always 1 anyway
	// TODO: Optimize the perfect mirror case more
	float n_dot_h = 0.f == sample.a2_ ? 1.f : std::sqrt((1.f - xi.y) / ((sample.a2_ - 1.f) * xi.y + 1.f));

	float sin_theta = std::sqrt(1.f - n_dot_h * n_dot_h);
	float phi = 2.f * math::Pi * xi.x;
	float sin_phi = std::sin(phi);
	float cos_phi = std::cos(phi);

	math::float3 is = math::float3(sin_theta * cos_phi, sin_theta * sin_phi, n_dot_h);
	math::float3 h = sample.tangent_to_world(is);

	float wo_dot_h = math::clamp(math::dot(sample.wo_, h), 0.00001f, 1.f);
//	float wo_dot_h = std::max(math::dot(sample.wo_, h), 0.00001f);

	math::float3 wi = math::normalized((2.f * wo_dot_h) * h - sample.wo_);

	float n_dot_wi = std::max(math::dot(sample.n_, wi),	  0.00001f);
//	float n_dot_wo = std::max(math::dot(sample.n_, BxDF<Sample>::sample_.wo_), 0.00001f);

	float d = distribution_isotropic(n_dot_h, std::max(sample.a2_, 0.0000001f));
	float g = geometric_shadowing(n_dot_wi, n_dot_wo, sample.a2_);
	math::float3 f = fresnel::schlick(wo_dot_h, sample.f0_);

	result.pdf = d * n_dot_h / (4.f * wo_dot_h);

	math::float3 specular = d * g * f;
	result.reflection = specular;
	result.wi = wi;
	result.type.clear_set(0.f == sample.a2_ ? bxdf::Type::Specular_reflection
											: bxdf::Type::Glossy_reflection);

//	if (wo_dot_h > 1.f) {
//		std::cout << "GGX<Sample>::importance_sample()" << std::endl;
//	}

//	if (math::contains_negative(result.reflection)) {
//		std::cout << "GGX<Sample>::importance_sample()" << std::endl;
//		std::cout << "d: " << d << std::endl;
//		std::cout << "g: " << g << std::endl;
//		std::cout << "f: " << f << std::endl;
//		std::cout << "wo_dot_h: " << wo_dot_h << std::endl;
//	}

	return n_dot_wi;
}

template<typename Sample>
math::float3 Conductor_isotropic<Sample>::evaluate(const Sample& sample,
												   const math::float3& wi, float n_dot_wi, float n_dot_wo,
												   float &pdf) const {
	// Roughness zero will always have zero specular term (or worse NaN)
	if (0.f == sample.a2_) {
		pdf = 0.f;
		return math::float3::identity;
	}

	math::float3 h = math::normalized(sample.wo_ + wi);

	float n_dot_h  = math::saturate(math::dot(sample.n_, h));
	float wo_dot_h = math::clamp(math::dot(sample.wo_, h), 0.00001f, 1.f);

	float d = distribution_isotropic(n_dot_h, std::max(sample.a2_, 0.0000001f));
	float g = geometric_shadowing(n_dot_wi, n_dot_wo, sample.a2_);
	math::float3 f = fresnel::conductor(wo_dot_h, sample.ior_, sample.absorption_);

	math::float3 specular = d * g * f;

//	if (math::contains_negative(specular)) {
//		std::cout << "GGX<Sample>::evaluate()" << std::endl;
//	}

	pdf = d * n_dot_h / (4.f * wo_dot_h);
	return specular;
}

template<typename Sample>
float Conductor_isotropic<Sample>::importance_sample(const Sample& sample,
													 sampler::Sampler& sampler, float n_dot_wo,
													 bxdf::Result& result) const {
	math::float2 xi = sampler.generate_sample_2D();

	// For zero roughness we risk NaN if xi.y == 1: n_dot_h is always 1 anyway
	// TODO: Optimize the perfect mirror case more
	float n_dot_h = 0.f == sample.a2_ ? 1.f : std::sqrt((1.f - xi.y) / ((sample.a2_ - 1.f) * xi.y + 1.f));

	float sin_theta = std::sqrt(1.f - n_dot_h * n_dot_h);
	float phi = 2.f * math::Pi * xi.x;
	float sin_phi = std::sin(phi);
	float cos_phi = std::cos(phi);

	math::float3 is = math::float3(sin_theta * cos_phi, sin_theta * sin_phi, n_dot_h);
	math::float3 h = sample.tangent_to_world(is);

//	float wo_dot_h = math::clamp(math::dot(sample.wo_, h), 0.00001f, 1.f);
	float wo_dot_h = std::max(math::dot(sample.wo_, h), 0.00001f);

	math::float3 wi = math::normalized((2.f * wo_dot_h) * h - sample.wo_);

	float n_dot_wi = std::max(math::dot(sample.n_, wi),	  0.00001f);
//	float n_dot_wo = std::max(math::dot(sample.n_, BxDF<Sample>::sample_.wo_), 0.00001f);

	float d = distribution_isotropic(n_dot_h, std::max(sample.a2_, 0.0000001f));
	float g = geometric_shadowing(n_dot_wi, n_dot_wo, sample.a2_);
	math::float3 f = fresnel::conductor(wo_dot_h, sample.ior_, sample.absorption_);

	result.pdf = d * n_dot_h / (4.f * wo_dot_h);

	math::float3 specular = d * g * f;
	result.reflection = specular;
	result.wi = wi;
	result.type.clear_set(0.f == sample.a2_ ? bxdf::Type::Specular_reflection
											: bxdf::Type::Glossy_reflection);

//	if (math::contains_negative(result.reflection)) {
//		std::cout << "GGX<Sample>::importance_sample()" << std::endl;
//		std::cout << "d: " << d << std::endl;
//		std::cout << "g: " << g << std::endl;
//		std::cout << "f: " << f << std::endl;
//		std::cout << "wo_dot_h: " << wo_dot_h << std::endl;
//	}

	return n_dot_wi;
}

template<typename Sample>
math::float3 Conductor_anisotropic<Sample>::evaluate(const Sample& sample,
													 const math::float3& wi, float n_dot_wi, float n_dot_wo,
													 float &pdf) const {
	math::float3 h = math::normalized(sample.wo_ + wi);

	float n_dot_h  = math::saturate(math::dot(sample.n_, h));

//	float x_dot_h  = math::saturate(math::dot(sample.t_, h));
//	float y_dot_h  = math::saturate(math::dot(sample.b_, h));

	float x_dot_h  = math::dot(sample.t_, h);
	float y_dot_h  = math::dot(sample.b_, h);

	float wo_dot_h = math::clamp(math::dot(sample.wo_, h), 0.00001f, 1.f);

//	float d = ggx::d(n_dot_h, std::max(sample.a2_, 0.0000001f));
	float d = distribution_anisotropic(n_dot_h, x_dot_h, y_dot_h, sample.a2_, sample.axy_);
	float g = geometric_shadowing(n_dot_wi, n_dot_wo, sample.axy_);
	math::float3 f = fresnel::conductor(wo_dot_h, sample.ior_, sample.absorption_);

	math::float3 specular = d * g * f;

//	if (math::contains_negative(specular)) {
//		std::cout << "GGX<Sample>::evaluate()" << std::endl;
//	}

	pdf = d * n_dot_h / (4.f * wo_dot_h);
	return specular;
}

template<typename Sample>
float Conductor_anisotropic<Sample>::importance_sample(const Sample& sample,
													   sampler::Sampler& sampler, float n_dot_wo,
													   bxdf::Result& result) const {
	math::float2 xi = sampler.generate_sample_2D();

	float phi = 2.f * math::Pi * xi.x;
	float sin_phi = std::sin(phi);
	float cos_phi = std::cos(phi);

	float t0 = std::sqrt(xi.y / (1.f - xi.y));
	math::float3 t1 = sample.a_.x * cos_phi * sample.t_ + sample.a_.y * sin_phi * sample.b_;

	math::float3 h = math::normalized(t0 * t1 + sample.n_);

	float x_dot_h = math::dot(sample.t_, h);
	float y_dot_h = math::dot(sample.b_, h);
	float n_dot_h = math::dot(sample.n_, h);

//	float wo_dot_h = math::clamp(math::dot(sample.wo_, h), 0.00001f, 1.f);
	float wo_dot_h = std::max(math::dot(sample.wo_, h), 0.00001f);

	math::float3 wi = math::normalized((2.f * wo_dot_h) * h - sample.wo_);

	float n_dot_wi = std::max(math::dot(sample.n_, wi),	  0.00001f);
//	float n_dot_wo = std::max(math::dot(sample.n_, BxDF<Sample>::sample_.wo_), 0.00001f);

//	float d = ggx::d(n_dot_h, std::max(sample.a2_, 0.0000001f));
	float d = distribution_anisotropic(n_dot_h, x_dot_h, y_dot_h, sample.a2_, sample.axy_);
	float g = geometric_shadowing(n_dot_wi, n_dot_wo, sample.axy_);
	math::float3 f = fresnel::conductor(wo_dot_h, sample.ior_, sample.absorption_);

	result.pdf = d * n_dot_h / (4.f * wo_dot_h);

	math::float3 specular = d * g * f;
	result.reflection = specular;
	result.wi = wi;
	result.type.clear_set(bxdf::Type::Glossy_reflection);

//	if (math::contains_negative(result.reflection)) {
//		std::cout << "GGX<Sample>::importance_sample()" << std::endl;
//		std::cout << "d: " << d << std::endl;
//		std::cout << "g: " << g << std::endl;
//		std::cout << "f: " << f << std::endl;
//		std::cout << "wo_dot_h: " << wo_dot_h << std::endl;
//	}

	return n_dot_wi;
}

inline float distribution_isotropic(float n_dot_h, float a2) {
	float d = n_dot_h * n_dot_h * (a2 - 1.f) + 1.f;
	return a2 / (math::Pi * d * d);
}

inline float distribution_anisotropic(float n_dot_h, float x_dot_h, float y_dot_h, math::float2 a2, float axy) {
	float x = (x_dot_h * x_dot_h) / a2.x;
	float y = (y_dot_h * y_dot_h) / a2.y;
	float d = (x + y + n_dot_h * n_dot_h);

	return 1.f / (math::Pi * axy * d * d);
}

inline float geometric_shadowing(float n_dot_wi, float n_dot_wo, float a2) {
	float g_wo = n_dot_wo + std::sqrt((n_dot_wo - n_dot_wo * a2) * n_dot_wo + a2);
	float g_wi = n_dot_wi + std::sqrt((n_dot_wi - n_dot_wi * a2) * n_dot_wi + a2);
	return 1.f / (g_wo * g_wi);
}

}}}
