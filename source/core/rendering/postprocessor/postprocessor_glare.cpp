#include "postprocessor_glare.hpp"
#include "image/typed_image.inl"
#include "base/memory/align.hpp"
#include "base/math/exp.hpp"
#include "base/math/vector4.inl"
#include "base/math/filter/gaussian.hpp"
#include "base/spectrum/interpolated.hpp"
#include "base/spectrum/rgb.hpp"
#include "base/spectrum/xyz.hpp"
#include "base/thread/thread_pool.hpp"
#include <vector>

#include "base/math/print.hpp"
#include <iostream>

namespace rendering { namespace postprocessor {

Glare::Glare(Adaption adaption, float threshold, float intensity) :
	Postprocessor(2),
	adaption_(adaption),
	threshold_(threshold), intensity_(intensity),
	high_pass_(nullptr),
	kernel_(nullptr) {}

Glare::~Glare() {
	memory::free_aligned(kernel_);
	memory::free_aligned(high_pass_);
}

float f0(float theta) {
	float b = theta / 0.02f;
	return 2.61f * 10e6f * math::exp(-(b * b));
}

float f1(float theta) {
	float b = 1.f / (theta + 0.02f);
	return 20.91f * b * b * b;
}

float f2(float theta) {
	float b = 1.f / (theta + 0.02f);
	return 72.37f * b * b;
}

float f3(float theta, float lambda) {
	float b = theta - 3.f * (lambda / 568.f);
	return 436.9f * (568.f / lambda) * math::exp(-(b * b));
}

void Glare::init(const scene::camera::Camera& camera, thread::Pool& pool) {
	const auto dim = camera.sensor_dimensions();
	dimensions_ = dim;
	high_pass_ = memory::allocate_aligned<float3>(dim[0] * dim[1]);

	// This seems a bit arbitrary
	const float solid_angle = 0.5f * math::radians_to_degrees(camera.pixel_solid_angle());

	kernel_dimensions_ = 2 * dim;
	int32_t kernel_size = kernel_dimensions_[0] * kernel_dimensions_[1];
	kernel_ = memory::allocate_aligned<float3>(kernel_size);

	const spectrum::Interpolated CIE_X(spectrum::CIE_Wavelengths_360_830_1nm, spectrum::CIE_X_360_830_1nm, spectrum::CIE_XYZ_Num);
	const spectrum::Interpolated CIE_Y(spectrum::CIE_Wavelengths_360_830_1nm, spectrum::CIE_Y_360_830_1nm, spectrum::CIE_XYZ_Num);
	const spectrum::Interpolated CIE_Z(spectrum::CIE_Wavelengths_360_830_1nm, spectrum::CIE_Z_360_830_1nm, spectrum::CIE_XYZ_Num);

	constexpr float wl_start = 400.f;
	constexpr float wl_end = 700.f;
	constexpr int32_t wl_num_samples = 64;
	constexpr float wl_step = (wl_end - wl_start) / static_cast<float>(wl_num_samples);
	const	  float wl_norm = 1.f / CIE_Y.integrate(wl_start, wl_end);

	struct F {
		float  a;
		float  b;
		float  c;
		float3 d;
	};

	std::vector<F> f(kernel_size);

	struct Init {
		float  a_sum = 0.f;
		float  b_sum = 0.f;
		float  c_sum = 0.f;
		float3 d_sum = float3(0.f);
	};

	std::vector<Init> inits(pool.num_threads());

	pool.run_range([this, dim, solid_angle,  wl_start, wl_step, wl_norm,
				   wl_num_samples, &CIE_X, &CIE_Y, &CIE_Z, &f, &inits]
				   (uint32_t id, int32_t begin, int32_t end) {
		Init& init = inits[id];

		for (int32_t y = begin; y < end; ++y) {
			for (int32_t x = 0; x < kernel_dimensions_[0]; ++x) {
				int2 p(-dim[0] + x, -dim[1] + y);

				float theta = math::length(float2(p)) * solid_angle;

				float a = f0(theta);
				float b = f1(theta);
				float c = f2(theta);

				init.a_sum += a;
				init.b_sum += b;
				init.c_sum += c;

				float3 d(0.f);

				if (Adaption::Photopic != adaption_) {
					float3 xyz(0.f);
					for (int32_t k = 0; k < wl_num_samples; ++k) {
						float lambda = wl_start + static_cast<float>(k) * wl_step;
						float val = wl_norm * f3(theta , lambda);
						xyz[0] += CIE_X.evaluate(lambda) * val;
						xyz[1] += CIE_Y.evaluate(lambda) * val;
						xyz[2] += CIE_Z.evaluate(lambda) * val;
					}

					d = math::max(spectrum::XYZ_to_linear_RGB(xyz), float3(0.f));

					init.d_sum += d;
				}

				int32_t i = y * kernel_dimensions_[0] + x;
				f[i] = F{ a, b, c, d };
			}
		}
	}, 0, kernel_dimensions_[1]);

	float a_sum = 0.f;
	float b_sum = 0.f;
	float c_sum = 0.f;
	float3 d_sum(0.f);

	for (auto i : inits) {
		a_sum += i.a_sum;
		b_sum += i.b_sum;
		c_sum += i.c_sum;
		d_sum += i.d_sum;
	}

	float scale[4];

	switch (adaption_) {
	case Adaption::Scotopic:
		scale[0] = 0.282f;
		scale[1] = 0.478f;
		scale[2] = 0.207f;
		scale[3] = 0.033f;
		break;
	default:
	case Adaption::Mesopic:
		scale[0] = 0.368f;
		scale[1] = 0.478f;
		scale[2] = 0.138f;
		scale[3] = 0.016f;
		break;
	case Adaption::Photopic:
		scale[0] = 0.383f;
		scale[1] = 0.478f;
		scale[2] = 0.138f;
		scale[3] = 0.f;
		break;
	}

	float a_n = scale[0] / a_sum;
	float b_n = scale[1] / b_sum;
	float c_n = scale[2] / c_sum;

	if (Adaption::Photopic == adaption_) {
		for (int32_t i = 0, len = kernel_size; i < len; ++i) {
			kernel_[i] = float3(a_n * f[i].a + b_n * f[i].b + c_n * f[i].c);
		}
	} else {
		float3 d_n = scale[3] / d_sum;

		for (int32_t i = 0, len = kernel_size; i < len; ++i) {
			kernel_[i] = float3(a_n * f[i].a + b_n * f[i].b + c_n * f[i].c) + d_n * f[i].d;
		}
	}
}

size_t Glare::num_bytes() const {
	return sizeof(*this) +
			(dimensions_[0] * dimensions_[1]) * sizeof(float3) +
			(kernel_dimensions_[0] * kernel_dimensions_[1]) * sizeof(float3);
}

void Glare::apply(int32_t begin, int32_t end, uint32_t pass,
				  const image::Float_4& source, image::Float_4& destination) {
	if (0 == pass) {
		float threshold = threshold_;

		for (int32_t i = begin; i < end; ++i) {
			float3 color = source.at(i).xyz();

			float l = spectrum::luminance(color);

			if (l > threshold) {
				high_pass_[i] = color;
			} else {
				high_pass_[i] = float3(0.f);
			}
		}
	} else {
		float intensity = intensity_;

		const auto d = destination.description().dimensions.xy();

		int2 kd = kernel_dimensions_;
		int2 hkd = kd / 2;

		for (int32_t i = begin; i < end; ++i) {
			int2 c = destination.coordinates_2(i);
			int2 cd = c - d;
			int2 kb = hkd - c;
			int2 ke = kb + d;

			float3 glare(0.f);
			for (int32_t ky = kb[1]; ky < ke[1]; ++ky) {
				int32_t krow = ky * kd[0];
				int32_t srow = (cd[1] + ky) * d[0];
				for (int32_t kx = kb[0]; kx < ke[0]; ++kx) {
					float3 k = kernel_[krow + kx];

					int32_t sx = cd[0] + kx;
					glare += k * high_pass_[srow + sx];
				}
			}

			float4 s = source.load(i);

			destination.at(i) = float4(s.xyz() + intensity * glare, s[3]);
		}
	}
}

}}
