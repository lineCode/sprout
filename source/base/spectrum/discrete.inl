#ifndef SU_BASE_SPECTRUM_DISCRETE_INL
#define SU_BASE_SPECTRUM_DISCRETE_INL

#include "discrete.hpp"
#include "math/vector3.inl"
#include "xyz.hpp"

namespace spectrum {

template <int32_t N>
Discrete_spectral_power_distribution<N>::Discrete_spectral_power_distribution(
    Interpolated const& interpolated) noexcept {
    for (int32_t i = 0; i < N; ++i) {
        float const a = wavelengths_[i];
        float const b = wavelengths_[i + 1];
        values_[i]    = interpolated.integrate(a, b) / (b - a);
    }
}

template <int32_t N>
float constexpr Discrete_spectral_power_distribution<N>::value(int32_t bin) const noexcept {
    return values_[bin];
}

template <int32_t N>
void Discrete_spectral_power_distribution<N>::set_bin(int32_t bin, float value) noexcept {
    values_[bin] = value;
}

template <int32_t N>
void Discrete_spectral_power_distribution<N>::set_at_wavelength(float lambda,
                                                                float value) noexcept {
    if (lambda < wavelengths_[0] || lambda > wavelengths_[N]) {
        return;
    }

    uint32_t idx = 0;
    for (idx = 0; lambda > wavelengths_[idx + 1]; idx++) {
    }

    values_[idx] = value;
}

template <int32_t N>
void Discrete_spectral_power_distribution<N>::clear(float s) noexcept {
    for (uint32_t i = 0; i < N; ++i) {
        values_[i] = s;
    }
}

template <int32_t N>
float3 constexpr Discrete_spectral_power_distribution<N>::XYZ() const noexcept {
    float3 xyz(0.f);
    for (int32_t i = 0; i < N; ++i) {
        xyz += values_[i] * cie_[i];
    }

    return step_ * xyz;
}

template <int32_t N>
float3 constexpr Discrete_spectral_power_distribution<N>::normalized_XYZ() const noexcept {
    constexpr float normalization = 1.f / 106.856895f;
    return normalization * XYZ();
}

template <int32_t N>
void Discrete_spectral_power_distribution<N>::init(float start_wavelength,
                                                   float end_wavelength) noexcept {
    float const step = (end_wavelength - start_wavelength) / static_cast<float>(N);

    // initialize the wavelengths ranges of the bins
    for (int32_t i = 0; i < N + 1; ++i) {
        wavelengths_[i] = start_wavelength + static_cast<float>(i) * step;
    }

    step_ = step;

    Interpolated const CIE_X(CIE_Wavelengths_360_830_1nm, CIE_X_360_830_1nm, CIE_XYZ_Num);
    Interpolated const CIE_Y(CIE_Wavelengths_360_830_1nm, CIE_Y_360_830_1nm, CIE_XYZ_Num);
    Interpolated const CIE_Z(CIE_Wavelengths_360_830_1nm, CIE_Z_360_830_1nm, CIE_XYZ_Num);

    Discrete_spectral_power_distribution<N> const cie_x(CIE_X);
    Discrete_spectral_power_distribution<N> const cie_y(CIE_Y);
    Discrete_spectral_power_distribution<N> const cie_z(CIE_Z);

    for (int32_t i = 0; i < N; ++i) {
        cie_[i][0] = cie_x.values_[i];
        cie_[i][1] = cie_y.values_[i];
        cie_[i][2] = cie_z.values_[i];
    }
}

template <int32_t N>
int32_t constexpr Discrete_spectral_power_distribution<N>::num_bands() noexcept {
    return N;
}

template <int32_t N>
float constexpr Discrete_spectral_power_distribution<N>::wavelength_center(int32_t bin) noexcept {
    return (wavelengths_[bin] + wavelengths_[bin + 1]) * 0.5f;
}

template <int32_t N>
float constexpr Discrete_spectral_power_distribution<N>::start_wavelength() noexcept {
    return wavelengths_[0];
}

template <int32_t N>
float constexpr Discrete_spectral_power_distribution<N>::end_wavelength() noexcept {
    return wavelengths_[N];
}

template <int32_t N>
float3 Discrete_spectral_power_distribution<N>::cie_[N];

template <int32_t N>
float Discrete_spectral_power_distribution<N>::wavelengths_[N + 1];

template <int32_t N>
float Discrete_spectral_power_distribution<N>::step_;

}  // namespace spectrum

#endif
