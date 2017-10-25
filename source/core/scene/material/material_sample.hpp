#pragma once

#include "base/math/vector3.hpp"

namespace sampler { class Sampler; }

namespace scene {

class Worker;

namespace material {

namespace bxdf { struct Sample; }

class BSSRDF;

class Sample {

public:

	struct Layer {
		void set_tangent_frame(const float3& t, const float3& b, const float3& n);
		void set_tangent_frame(const float3& n);

		float clamp_n_dot(const float3& v) const;
		float clamp_abs_n_dot(const float3& v) const;
		float clamp_reverse_n_dot(const float3& v) const;

		const float3& shading_normal() const;

		float3 tangent_to_world(const float3& v) const;

		float3 world_to_tangent(const float3& v) const;

		float3 t_;
		float3 b_;
		float3 n_;
	};

	virtual const Layer& base_layer() const = 0;

	virtual float3 evaluate(const float3& wi, float& pdf) const = 0;

	virtual void sample(sampler::Sampler& sampler, bxdf::Sample& result) const = 0;

	virtual const BSSRDF& bssrdf(const Worker& worker) const;

	virtual float3 radiance() const;

	virtual float3 absorption_coeffecient() const;

	virtual float ior() const = 0;

	virtual bool is_pure_emissive() const;

	virtual bool is_transmissive() const;

	virtual bool is_translucent() const;

	virtual bool is_sss() const;

	const float3& wo() const;

	float clamp_geo_n_dot(const float3& v) const;
	float clamp_reverse_geo_n_dot(const float3& v) const;

	const float3& geometric_normal() const;

	bool same_hemisphere(const float3& v) const;

	void set_basis(const float3& geo_n, const float3& wo);

protected:

	float3 geo_n_;
	float3 wo_;
};

}}
