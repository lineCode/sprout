#include "flakes_provider.hpp"
#include "image/typed_image.inl"
#include "image/encoding/png/png_writer.hpp"
#include "image/procedural/image_renderer.hpp"
#include "base/math/vector.inl"
#include "base/math/sampling/sample_distribution.inl"
#include "base/math/sampling/sampling.inl"
#include "base/memory/variant_map.inl"
#include "base/random/generator.inl"
#include <iostream>

namespace image { namespace procedural { namespace flakes {

std::shared_ptr<Image> Provider::create_normal_map(const memory::Variant_map& options) {
	Properties props(options);

	Renderer renderer(props.dimensions, 4);

	std::shared_ptr<Image_byte_3> image = std::make_shared<Image_byte_3>(
				Image::Description(Image::Type::Byte_3, props.dimensions));

	renderer.set_brush(float3(0.f, 0.f, 1.f));
	renderer.clear();

	rnd::Generator rng(1, 2, 3, 4);

	uint32_t r_0 = rng.random_uint();

	for (uint32_t i = 0; i < props.num_flakes; ++i) {
		float2 s_0 = math::thing(i, props.num_flakes, r_0);
	//	float2 s = math::scrambled_hammersley(i, num_flakes, r_0);
	//	float2 s = math::ems(i, r_0, r_1);

		float2 s_1 = float2(rng.random_float(), rng.random_float());
		float3 normal = math::sample_hemisphere_uniform(s_1);

//		float3 normal(1.f, 1.f, 1.f);

		normal = math::normalized(normal + float3(0.f, 0.f, 1.f));

		renderer.set_brush(normal);

		float r_f = rng.random_float();
		renderer.draw_circle(s_0, props.radius + r_f * props.variance);
	}

	renderer.resolve(*image);

//	encoding::png::Writer::write("flakes_normal.png", *image);

	return image;
}

std::shared_ptr<Image> Provider::create_mask(const memory::Variant_map& options) {
	Properties props(options);

	Renderer renderer(props.dimensions, 4);

	std::shared_ptr<Image_byte_1> image = std::make_shared<Image_byte_1>(
				Image::Description(Image::Type::Byte_1, props.dimensions));

	renderer.set_brush(float3(0.f, 0.f, 0.f));
	renderer.clear();

	rnd::Generator rng(1, 2, 3, 4);

	uint32_t r_0 = rng.random_uint();

	renderer.set_brush(float3(1.f, 1.f, 1.f));

	for (uint32_t i = 0; i < props.num_flakes; ++i) {
		float2 s_0 = math::thing(i, props.num_flakes, r_0);

		// We do this to keep the rng in the same state as was used for the normals
		rng.random_float();
		rng.random_float();

		float r_f = rng.random_float();
		renderer.draw_circle(s_0, props.radius + r_f * props.variance);
	}

	renderer.resolve(*image);

//	encoding::png::Writer::write("flakes_mask.png", *image);

	return image;
}

Provider::Properties::Properties(const memory::Variant_map& options) :
	dimensions(512, 512) {
	float size = 0.006f;
	float density = 0.5f;
	options.query("size", size);
	options.query("density", density);

	float half_size = 0.5f * size;
	variance = half_size / 3.f;
	radius = half_size - variance;

	num_flakes = static_cast<uint32_t>(density / (size * size) + 0.5f);
}

}}}
