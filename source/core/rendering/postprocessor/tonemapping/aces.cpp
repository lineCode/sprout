#include "aces.hpp"
#include "base/math/vector.inl"

namespace rendering { namespace postprocessor { namespace tonemapping {

Aces::Aces(float hdr_max) :
	normalization_factor_(normalization_factor(hdr_max, tonemap_function(hdr_max))) {}

float3 Aces::tonemap(float3_p color) const {
	return normalization_factor_ * float3(tonemap_function(color.v[0]),
										  tonemap_function(color.v[1]),
										  tonemap_function(color.v[2]));
}

float Aces::tonemap_function(float x) {
	// ACES like in https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;

	return (x * (a * x + b)) / (x * (c * x + d) + e);
}

}}}
