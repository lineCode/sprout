#pragma once

#include "scene/material/material_sample.hpp"

namespace scene { namespace material { namespace matte {

class Sample : public material::Sample {

public:

	virtual const Layer& base_layer() const final override;

	virtual float3 evaluate(const float3& wi, float& pdf) const final override;

	virtual void sample(sampler::Sampler& sampler, bxdf::Result& result) const final override;

	virtual float ior() const final override;

	struct Layer : public material::Sample::Layer {
		void set(const float3& color);

		float3 diffuse_color_;
		float  roughness_;
		float  a2_;
	};

	Layer layer_;
};

}}}
