#include "starburst.hpp"
#include "aperture.hpp"
#include "dirt.hpp"
#include "core/image/typed_image.inl"
#include "core/image/encoding/png/png_reader.hpp"
#include "core/image/encoding/png/png_writer.hpp"
#include "core/image/filter/image_gaussian.inl"
#include "core/image/procedural/image_renderer.inl"
#include "base/math/vector.inl"
#include "base/math/fourier/dft.hpp"
#include "base/math/sampling/sample_distribution.inl"
#include "base/random/generator.inl"
#include "base/spectrum/discrete.inl"
#include "base/spectrum/rgb.inl"
#include "base/spectrum/xyz.inl"
#include "base/thread/thread_pool.hpp"

#include <iostream>
#include <fstream>
#include "base/math/print.hpp"

namespace procedural { namespace starburst {

static constexpr uint32_t Num_bands = 64;

using Spectrum = spectrum::Discrete_spectral_power_distribution<Num_bands>;

void render_dirt(image::Float_1& signal);

void render_aperture(const Aperture& aperture, image::Float_1& signal);

void centered_squared_magnitude(float* result, const float2* source, size_t width, size_t height);

void squared_magnitude(float* result, const float2* source, size_t width, size_t height);

void starburst(Spectrum* result, const float* source, int32_t bin, int32_t resolution);

void write_signal(const std::string& name, const image::Float_1& signal);


float2 sqrtc(float2 c) {
	float l = math::length(c);
	return 0.7071067f * float2(std::sqrt(l + c.x), std::sqrt(l - c.x) * math::sign(c.y));
}

float2 mulc(float2 a, float2 b) {
	return float2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

float2 mulc(float2 a, float t) {
	float c = std::cos(t);
	float s = std::sin(t);

	return float2(a.x * c - a.y * s, a.x * s + a.y * c);
}

void experiment_x(std::vector<float2>& destination, const image::Float_1& source) {
	auto d = source.description().dimensions;

	float sqrt_m = std::sqrt(static_cast<float>(d.x));



	float alpha = 0.3f;

	float cot = 1.f / std::tan(alpha * math::Pi * 0.5f);
	float csc = 1.f / std::sin(alpha * math::Pi * 0.5f);

	for (int32_t y = 0; y < d.y; ++y) {
		for (int32_t x = 0; x < d.x; ++x) {

			float u = (static_cast<float>(x) / static_cast<float>(d.x)) / sqrt_m;


			float2 integration(0.f);

			for (int32_t sx = 0; sx < d.x; ++sx) {


				int32_t i = y * d.x + sx;
				float v = source.load(i);

				float2 g(v, 0.f);
				float y = static_cast<float>(sx) * sqrt_m;
				float t = math::Pi * (cot * y * y - 2.f * csc * u * y);
				integration += mulc(g, t);
			}

			float2 s = mulc(sqrtc(float2(1.f, -cot)), math::Pi * cot * u * u);


			int32_t i = y * d.x + x;
			destination[i] = mulc(s, integration) / sqrt_m;

		}
	}
}

void experiment_y(std::vector<float2>& destination, const std::vector<float2>& source) {
	int2 d(512, 512);

	float sqrt_m = std::sqrt(static_cast<float>(d.x));



	float alpha = 0.3f;

	float cot = 1.f / std::tan(alpha * math::Pi * 0.5f);
	float csc = 1.f / std::sin(alpha * math::Pi * 0.5f);

	for (int32_t y = 0; y < d.y; ++y) {
		for (int32_t x = 0; x < d.x; ++x) {

			float u = (static_cast<float>(y) / static_cast<float>(d.x)) / sqrt_m;


			float2 integration(0.f);

			for (int32_t sy = 0; sy < d.y; ++sy) {


				int32_t i = sy * d.x + x;
				float2 g = source[i];

				float y = static_cast<float>(sy) * sqrt_m;
				float t = math::Pi * (cot * y * y - 2.f * csc * u * y);
				integration += mulc(g, t);
			}

			float2 s = mulc(sqrtc(float2(1.f, -cot)), math::Pi * cot * u * u);


			int32_t i = y * d.x + x;
			destination[i] = mulc(s, integration) / sqrt_m;

		}
	}
}

void create(thread::Pool& pool) {
	std::cout << "Starburst experiment" << std::endl;

	Spectrum::init(380.f, 720.f);

	int32_t resolution = 512;

	int2 dimensions(resolution, resolution);

	image::Float_1 signal(image::Image::Description(image::Image::Type::Float_1, dimensions));

	std::vector<float2> signal_f(resolution * math::dft_size(resolution));

	image::Float_3 float_image_a(image::Image::Description(image::Image::Type::Float_3,
														   dimensions));

	bool dirt = false;
	if (dirt) {
		render_dirt(signal);
	} else {
		signal.clear(1.f);
	}

	Aperture aperture(8, 0.25f);
	render_aperture(aperture, signal);

	write_signal("signal.png", signal);

	std::vector<float2> signal_e(resolution * resolution);
	std::vector<float2> signal_t(resolution * resolution);

	experiment_x(signal_e, signal);
	experiment_y(signal_t, signal_e);

	squared_magnitude(signal.data(), signal_t.data(), resolution, resolution);

	write_signal("signal_after.png", signal);

	return;

	math::dft_2d(signal_f.data(), signal.data(), resolution, resolution, pool);

	centered_squared_magnitude(signal.data(), signal_f.data(), resolution, resolution);

//	write_signal("signal_after.png", signal);

	Spectrum* spectral_data = new Spectrum[resolution * resolution];

	pool.run_range([spectral_data, &signal, resolution](int32_t begin, int32_t end) {
		for (int32_t bin = begin; bin < end; ++bin) {
			starburst(spectral_data, signal.data(), bin, resolution);
		}
	}, 0, Spectrum::num_bands());

	pool.run_range([spectral_data, &float_image_a](int32_t begin, int32_t end) {
		for (int32_t i = begin; i < end; ++i) {
			auto& s = spectral_data[i];
			float3 linear_rgb = spectrum::XYZ_to_linear_RGB(s.normalized_XYZ());
			float_image_a.store(i, math::packed_float3(linear_rgb));
		}
	}, 0, resolution * resolution);

	delete [] spectral_data;

	float radius = static_cast<float>(resolution) * 0.00390625f;
	image::filter::Gaussian<math::packed_float3> gaussian(radius, radius * 0.0005f);
	gaussian.apply(float_image_a);

	image::Byte_3 byte_image(image::Image::Description(image::Image::Type::Byte_3,
													   dimensions));

	pool.run_range([&float_image_a, &byte_image](int32_t begin, int32_t end) {
		for (int32_t i = begin; i < end; ++i) {
			float3 linear_rgb = float3(float_image_a.load(i));
			byte3 srgb = spectrum::float_to_unorm(spectrum::linear_RGB_to_sRGB(linear_rgb));
			byte_image.store(i, srgb);
		}
	}, 0, resolution * resolution);

	image::encoding::png::Writer::write("starburst.png", byte_image);
}

void render_dirt(image::Float_1& signal) {
//	std::ifstream stream("lens_dirt.png", std::ios::binary);

//	try {
//		auto image = image::encoding::png::Reader::read(stream, image::Channels::X, 1);

//		if (image::Image::Type::Byte_1 != image->description().type) {
//			std::cout << "wrong type" << std::endl;
//		}

//		const image::Byte_1* byte_image = dynamic_cast<const image::Byte_1*>(image.get());

//		if (signal.description().dimensions != image->description().dimensions) {
//			std::cout << "dimensions not matching" << std::endl;
//			return;
//		}

//		for (int32_t i = 0, len = signal.area(); i < len; ++i) {
//			uint8_t byte = byte_image->load(i);
//			float linear = spectrum::unorm_to_float(byte);
//			signal.store(i, std::pow(linear, 4.f));
//		}

//	} catch (std::exception& e) {
//		std::cout << e.what() << std::endl;
//	}

//	return;

	Dirt dirt(signal.description().dimensions.xy, 4);

	dirt.set_brush(1.f);
	dirt.clear();

	rnd::Generator rng(0, 1, 2, 3);

	float dirt_radius = 0.003f;

	uint32_t num_base_dirt = 128;

	uint32_t base_scramble = rng.random_uint();

	for (uint32_t i = 0; i < num_base_dirt; ++i) {
		float2 p = math::hammersley(i, num_base_dirt, base_scramble);

		float rc = rng.random_float();
		dirt.set_brush(0.8f + 0.2f * rc);

		float rs = rng.random_float();
		dirt.draw_circle(p, dirt_radius + 32.f * rs * dirt_radius);
	}

	uint32_t num_detail_dirt = 2 * 1024;

	uint32_t detail_scramble = rng.random_uint();

	for (uint32_t i = 0; i < num_detail_dirt; ++i) {
		float2 p = math::hammersley(i, num_detail_dirt, detail_scramble);

		float rc = rng.random_float();
		dirt.set_brush(0.9f + 0.1f * rc);

		float rs = rng.random_float();
		dirt.draw_circle(p, dirt_radius + 2.f * rs * dirt_radius);
	}

	float inner = 1.f;
	float outer = 0.75f;

	dirt.draw_concentric_circles(float2(0.7f, 0.25f), 12, 0.005f, inner, outer);
	dirt.draw_concentric_circles(float2(0.4f, 0.6f), 12, 0.005f, inner, outer);
	dirt.draw_concentric_circles(float2(0.6f, 0.8f), 12, 0.005f, inner, outer);

	dirt.resolve(signal);

//	float radius = static_cast<float>(signal.description().dimensions.x) * 0.00390625f;
//	image::filter::Gaussian<float> gaussian(radius, radius * 0.0005f);
//	gaussian.apply(signal);
}

void render_aperture(const Aperture& aperture, image::Float_1& signal) {
	int32_t num_sqrt_samples = 4;
	std::vector<float2> kernel(num_sqrt_samples * num_sqrt_samples);

	float kd = 1.f / static_cast<float>(num_sqrt_samples);

	for (int32_t y = 0; y < num_sqrt_samples; ++y) {
		for (int32_t x = 0; x < num_sqrt_samples; ++x) {
			float2 k(0.5f * kd + static_cast<float>(x) * kd,
					 0.5f * kd + static_cast<float>(y) * kd);

			kernel[y * num_sqrt_samples + x] = k;
		}
	}

	int32_t resolution = signal.description().dimensions.x;
	float fr = static_cast<float>(resolution);
	float kn = 1.f / static_cast<float>(kernel.size());

	float radius = static_cast<float>(resolution - 2) / fr;

	for (int32_t y = 0; y < resolution; ++y) {
		for (int32_t x = 0; x < resolution; ++x) {
			float s = signal.load(x, y);

			float fx = static_cast<float>(x);
			float fy = static_cast<float>(y);

			float a = 0.f;
			for (auto k : kernel) {
				float2 p(-1.f + 2.f * ((fx + k.x) / fr),
						  1.f - 2.f * ((fy + k.y) / fr));

				a += kn * aperture.evaluate(p, radius);
			}

			signal.store(x, y, s * a);
		}
	}
}

void centered_squared_magnitude(float* result, const float2* source, size_t width, size_t height) {
	size_t row_size = math::dft_size(width);

	for (size_t y = 0; y < height; ++y) {
		size_t ro = y;

		if (y < height / 2) {
			ro = y + height / 2;
		} else {
			ro = y - height / 2;
		}


//		size_t o = y * row_size;
//		float mag = /*0.001f **/ math::length(source[o]);

//		result[ro * width + row_size - 1] = mag;


		for (size_t x = 0, len = row_size - 1; x < len; ++x) {
			size_t o = y * row_size + x;
			float mag = /*0.001f **/ math::squared_length(source[o]);

//			size_t a = ro * width + x + len;
//			std::cout << a << std::endl;

			result[ro * width + x + len] = mag;
		}

		for (size_t x = 0, len = row_size - 1; x < len; ++x) {
			size_t o = y * row_size + x;
			float mag = /*0.001f **/ math::squared_length(source[o]);

//			size_t a = ro * width - x + len - 1;
//			std::cout << a << std::endl;

			result[ro * width - x + len - 1] = mag;
		}

	//	std::cout << "row end" << std::endl;
	}
}

void squared_magnitude(float* result, const float2* source, size_t width, size_t height) {
	for (size_t y = 0; y < height; ++y) {
		for (size_t x = 0; x < width; ++x) {
			size_t i = y * width + x;
			float mag = math::squared_length(source[i]);
			result[i] = mag;
		}
	}
}

void starburst(Spectrum* result, const float* squared_magnitude, int32_t bin, int32_t resolution) {
	float fr = static_cast<float>(resolution);

	float wl_0 = Spectrum::wavelength_center(Spectrum::num_bands() - 1);
//	float wl_0 = Spectrum::wavelength_center(Spectrum::num_bands() / 2);

	float wl = Spectrum::wavelength_center(bin);

	float i_s = wl / wl_0;

	float normalization = (i_s * i_s) * (2.f / (fr * fr));

	for (int32_t y = 0; y < resolution; ++y) {
		for (int32_t x = 0; x < resolution; ++x) {
			float2 p(-1.f + 2.f * ((static_cast<float>(x) + 0.5f) / fr),
					  1.f - 2.f * ((static_cast<float>(y) + 0.5f) / fr));

			float2 q = i_s * p;

			q.x =  0.5f * (q.x + 1.f);
			q.y = -0.5f * (q.y - 1.f);

		//	q = math::saturate(q);

			float r;

			if (q.x < 0.f || q.x >= 1.f
			||  q.y < 0.f || q.y >= 1.f) {
				r = 0.f;
			} else {
				float dp = math::dot(p, p);
				float v = std::max(1.f - dp * dp, 0.f);

				int32_t sx = static_cast<int32_t>(q.x * fr);
				int32_t sy = static_cast<int32_t>(q.y * fr);
				int32_t i = sy * resolution + sx;
				r = v * normalization * squared_magnitude[i];
			}

			int32_t o = y * resolution + x;
			result[o].set_bin(bin, r);
		}
	}
}

void write_signal(const std::string& name, const image::Float_1& signal) {
	auto d = signal.description().dimensions;

	image::Byte_1 image(image::Image::Description(image::Image::Type::Byte_1, d));

	for (int32_t i = 0, len = d.x * d.y; i < len; ++i) {
		float s = signal.load(i);
		uint8_t b = spectrum::float_to_unorm(s);
		image.store(i, b);
	}

	image::encoding::png::Writer::write(name, image);
}

}}
