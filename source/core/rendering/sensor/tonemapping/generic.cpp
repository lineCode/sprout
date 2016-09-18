#include "generic.hpp"
#include "base/math/vector.inl"

namespace rendering { namespace sensor { namespace tonemapping {

Generic::Generic(float contrast, float shoulder, float mid_in, float mid_out, float hdr_max) :
	a_(contrast),
	d_(shoulder),
	hdr_max_(hdr_max) {
	float ad = contrast * shoulder;

	float midi_pow_a  = std::pow(mid_in, contrast);
	float midi_pow_ad = std::pow(mid_in, ad);
	float hdrm_pow_a  = std::pow(hdr_max, contrast);
	float hdrm_pow_ad = std::pow(hdr_max, ad);

	float u = hdrm_pow_ad * mid_out - midi_pow_ad * mid_out;
	float v = midi_pow_ad * mid_out;

	b_ = -((-midi_pow_a + (mid_out * (hdrm_pow_ad * midi_pow_a - hdrm_pow_a * v)) / u) / v);
	c_ = (hdrm_pow_ad * midi_pow_a - hdrm_pow_a * v) / u;
}

float3 Generic::tonemap(float3_p color) const {
//	float peak = math::max_element(color);

//	float3 ratio = color / peak;

//	return tonemap_function(peak) * ratio;

	return float3(tonemap_function(color.x),
				  tonemap_function(color.y),
				  tonemap_function(color.z));
}

float Generic::tonemap_function(float x) const {
	x = std::min(x, hdr_max_);
	float z = std::pow(x, a_);
	return z / (std::pow(z, d_) * b_ + c_);
}

}}}
