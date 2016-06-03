#pragma once

#include "math/vector.hpp"

namespace spectrum {

template<uint32_t N>
class Discrete_spectral_power_distribution {

public:

	void set_bin(uint32_t bin, float value);

	math::float3 xyz() const;

	static void init(float start_wavelength = 380.f, float end_wavelength = 720.f);

private:

	static float start_wavelength();
	static float end_wavelength();

	float values_[N];

	static float wavelengths_[N  + 1];

	static Discrete_spectral_power_distribution cie_x_;
	static Discrete_spectral_power_distribution cie_y_;
	static Discrete_spectral_power_distribution cie_z_;
};

}
