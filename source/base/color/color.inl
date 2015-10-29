#pragma once

#include "color.hpp"
#include "math/vector.inl"

namespace color {

// convert sRGB linear value to sRGB gamma value
inline float linear_to_sRGB(float c) {
	if (c <= 0.f) {
		return 0.f;
	} else if (c < 0.0031308f) {
		return 12.92f * c;
	} else if (c < 1.f) {
		return 1.055f * std::pow(c, 0.41666f) - 0.055f;
	}

	return 1.f;
}

// convert sRGB linear color to sRGB gamma color
inline Color3 linear_to_sRGB(const Color3& c) {
	return Color3(linear_to_sRGB(c.x), linear_to_sRGB(c.y), linear_to_sRGB(c.z));
}

// convert sRGB gamma value to sRGB linear value
inline float sRGB_to_linear(float c) {
	if (c <= 0.f) {
		return 0.f;
	} else if (c < 0.04045f) {
		return c / 12.92f;
	} else if (c < 1.f) {
		return std::pow((c + 0.055f) / 1.055f, 2.4f);
	}

	return 1.f;
}

// convert sRGB gamma color to sRGB linear color
inline Color3 sRGB_to_linear(const Color3c& c) {
	return Color3(sRGB_to_linear(static_cast<float>(c.x) / 255.f),
				  sRGB_to_linear(static_cast<float>(c.y) / 255.f),
				  sRGB_to_linear(static_cast<float>(c.z) / 255.f));
}

// convert sRGB gamma color to sRGB linear color
inline Color3 sRGB_to_linear(const Color3& c) {
	return Color3(sRGB_to_linear(c.x), sRGB_to_linear(c.y), sRGB_to_linear(c.z));
}

inline Color4 sRGB_to_linear(const Color4c& c) {
	return Color4(sRGB_to_linear(static_cast<float>(c.x) / 255.f),
				  sRGB_to_linear(static_cast<float>(c.y) / 255.f),
				  sRGB_to_linear(static_cast<float>(c.z) / 255.f),
				  static_cast<float>(c.w) / 255.f);
}

// convert linear color to gamma color
inline Color3 linear_to_gamma(const Color3& c, float gamma) {
	float p = 1.f / gamma;

	return Color3(std::pow(c.x, p), std::pow(c.y, p), std::pow(c.z, p));
}

// convert gamma color to linear color
inline Color3 gamma_to_linear(const Color3& c, float gamma) {
	return Color3(std::pow(c.x, gamma), std::pow(c.y, gamma), std::pow(c.z, gamma));
}

inline Color3 to_float(const Color3c& c) {
	return Color3(static_cast<float>(c.x) / 255.f,
				  static_cast<float>(c.y) / 255.f,
				  static_cast<float>(c.z) / 255.f);
}

inline Color4 to_float(const Color4c& c) {
	return Color4(static_cast<float>(c.x) / 255.f,
				  static_cast<float>(c.y) / 255.f,
				  static_cast<float>(c.z) / 255.f,
				  static_cast<float>(c.w) / 255.f);
}

inline Color4c to_byte(const Color4& c) {
	return Color4c(static_cast<unsigned char>(c.x * 255.f),
				   static_cast<unsigned char>(c.y * 255.f),
				   static_cast<unsigned char>(c.z * 255.f),
				   static_cast<unsigned char>(c.w * 255.f));
}

inline float snorm_to_float(unsigned char byte) {
	return static_cast<float>(byte) / 128.f - 1.f;
}

inline float unorm_to_float(unsigned char byte) {
	return static_cast<float>(byte) / 255.f;
}

const Color3 luminance_vector(0.299f, 0.587f, 0.114f);

inline float luminance(const Color3& c) {
	return dot(c, luminance_vector);
}

}
