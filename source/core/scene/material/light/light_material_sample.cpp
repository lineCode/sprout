#include "light_material_sample.hpp"
#include "sampler/sampler.hpp"
#include "base/math/vector.inl"

namespace scene { namespace material { namespace light {

Sample::Sample() {}

math::float3 Sample::evaluate(const math::float3& /*wi*/, float& pdf) const {
	pdf = 0.f;
	return math::float3::identity;
}

math::float3 Sample::emission() const {
	return emission_;
}

math::float3 Sample::attenuation() const {
	return math::float3(100.f, 100.f, 100.f);
}

void Sample::sample_evaluate(sampler::Sampler& /*sampler*/, bxdf::Result& result) const {
	result.reflection = math::float3::identity;
	result.pdf = 0.f;

	result.type.clear_set(bxdf::Type::Diffuse_reflection);
}

bool Sample::is_pure_emissive() const {
	return true;
}

bool Sample::is_translucent() const {
	return false;
}

void Sample::set(const math::float3& emission) {
	emission_ = emission;
}

}}}