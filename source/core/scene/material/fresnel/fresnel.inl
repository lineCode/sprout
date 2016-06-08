#pragma once

#include "fresnel.hpp"
#include "base/math/vector.inl"

namespace scene { namespace material { namespace fresnel {

inline math::float3 schlick(float wo_dot_h, math::pfloat3 f0) {
	return f0 + std::pow(1.f - wo_dot_h, 5.f) * (1.f - f0);

	// Gaussian approximation
	// return f0 + (std::exp2((-5.55473f * wo_dot_h - 6.98316f) * wo_dot_h)) * omf;
}

inline float schlick_f0(float n0, float n1) {
	float t = (n0 - n1) / (n0 + n1);
	return t * t;
}

inline math::float3 conductor(float wo_dot_h, math::pfloat3 eta, math::pfloat3 k) {
	math::float3 tmp_f = eta * eta + k * k;

	float wo_dot_h2 = wo_dot_h * wo_dot_h;
	math::float3 tmp = wo_dot_h2 * tmp_f;

	math::float3 a = 2.f * wo_dot_h * eta;
	math::float3 r_p = (tmp - a + 1.f)
					 / (tmp + a + 1.f);

	math::float3 r_o = (tmp_f - a + wo_dot_h2)
					 / (tmp_f + a + wo_dot_h2);

	return 0.5f * (r_p + r_o);
}

// Adapted from https://seblagarde.wordpress.com/2013/04/29/memo-on-fresnel-equations/#more-1921
inline float dielectric(float n_dot_wi, float n_dot_wo, float eta) {
	float t1 = eta * n_dot_wo;
	float t2 = eta * n_dot_wi;

	float rs = (n_dot_wi + t1) / (n_dot_wi - t1);
	float rp = (n_dot_wo + t2) / (n_dot_wo - t2);

	return 0.5f * (rs * rs + rp * rp);
}

inline float dielectric_holgerusan(float cos_theta_i, float cos_theta_t, float eta_i, float eta_t) {
	float r_p = (eta_t * cos_theta_i + eta_i * cos_theta_t)
			  / (eta_t * cos_theta_i - eta_i * cos_theta_t);

	float r_o = (eta_i * cos_theta_i + eta_t * cos_theta_t)
			  / (eta_i * cos_theta_i - eta_t * cos_theta_t);

	return 0.5f * (r_p * r_p + r_o * r_o);
}

// Amplitude reflection coefficient (s-polarized)
inline float rs(float n1, float n2, float cosI, float cosT) {
	return (n1 * cosI - n2 * cosT) / (n1 * cosI + n2 * cosT);
}

// Amplitude reflection coefficient (p-polarized)
inline float rp(float n1, float n2, float cosI, float cosT) {
	return (n2 * cosI - n1 * cosT) / (n1 * cosT + n2 * cosI);
}

// Amplitude transmission coefficient (s-polarized)
inline float ts(float n1, float n2, float cosI, float cosT) {
	return 2.f * n1 * cosI / (n1 * cosI + n2 * cosT);
}

// Amplitude transmission coefficient (p-polarized)
inline float tp(float n1, float n2, float cosI, float cosT) {
	return 2.f * n1 * cosI / (n1 * cosT + n2 * cosI);
}

inline math::float3 thinfilm(float wo_dot_h, float external_ior, float thinfilm_ior,
							 float internal_ior, float thickness) {
	// Precompute the reflection phase changes (depends on IOR)
	float delta10 = (thinfilm_ior < external_ior) ? math::Pi : 0.f;
	float delta12 = (thinfilm_ior < internal_ior) ? math::Pi : 0.f;
	float delta = delta10 + delta12;

	// Calculate the thin film layer (and transmitted) angle cosines.
	float a = 1.f - wo_dot_h * wo_dot_h;

	float eta_i = external_ior / internal_ior;
	float eta_t = external_ior / thinfilm_ior;

	float sin1 = eta_t * eta_t * a;
	float sin2 = eta_i * eta_i * a;

	if (sin1 > 1.f || sin2 > 1.f) {
		return math::float3(1.f); // Account for TIR.
	}

	float cos1 = std::sqrt(1.f - sin1);
	float cos2 = std::sqrt(1.f - sin2);

	// Calculate the interference phase change.
	math::float3 phi = math::float3(2.f * thinfilm_ior * thickness * cos1);
	phi *= 2.f * math::Pi / math::float3(650, 510, 475);
	phi += math::float3(delta);

	// Obtain the various Fresnel amplitude coefficients.
	float alpha_s = rs(thinfilm_ior, external_ior, cos1, wo_dot_h)
				  * rs(thinfilm_ior, internal_ior, cos1, cos2);
	float alpha_p = rp(thinfilm_ior, external_ior, cos1, wo_dot_h)
				  * rp(thinfilm_ior, internal_ior, cos1, cos2);
	float beta_s  = ts(external_ior, thinfilm_ior, wo_dot_h, cos1)
				  * ts(thinfilm_ior, internal_ior, cos1, cos2);
	float beta_p  = tp(external_ior, thinfilm_ior, wo_dot_h, cos1)
				  * tp(thinfilm_ior, internal_ior, cos1, cos2);

	// Calculate the s- and p-polarized intensity transmission coefficient
	math::float3 cos_phi = math::cos(phi);
	math::float3 ts = (beta_s * beta_s) / ((alpha_s * alpha_s) - 2.f * alpha_s * cos_phi + 1.f);
	math::float3 tp = (beta_p * beta_p) / ((alpha_p * alpha_p) - 2.f * alpha_p * cos_phi + 1.f);

	// Calculate the transmitted power ratio for medium change.
	float beam_ratio = (internal_ior * cos2) / (external_ior * wo_dot_h);

	// Calculate the average reflectance.
	return math::float3(1.f) - beam_ratio * 0.5f * (ts + tp);
}

inline Schlick::Schlick(math::pfloat3 f0) : f0_(f0) {}

inline math::float3 Schlick::f(float wo_dot_h) const {
	return schlick(wo_dot_h, f0_);
}

inline Thinfilm::Thinfilm(float external_ior, float thinfilm_ior,
						  float internal_ior, float thickness) :
	external_ior_(external_ior), thinfilm_ior_(thinfilm_ior),
	internal_ior_(internal_ior), thickness_(thickness) {}

inline math::float3 Thinfilm::f(float wo_dot_h) const {
	return thinfilm(wo_dot_h, external_ior_, thinfilm_ior_, internal_ior_, thickness_);
}

inline Conductor::Conductor(math::pfloat3 eta, math::pfloat3 k) : eta_(eta), k_(k) {}

inline math::float3 Conductor::f(float wo_dot_h) const {
	return conductor(wo_dot_h, eta_, k_);
}

}}}
