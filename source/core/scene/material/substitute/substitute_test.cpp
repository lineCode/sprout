#include "substitute_test.hpp"
#include "substitute_sample.hpp"
#include "scene/material/material_print.hpp"
#include "sampler/random_sampler.hpp"
#include "base/math/random/generator.inl"
#include "base/math/vector.inl"
#include "base/math/print.hpp"
#include <iostream>

namespace scene { namespace material { namespace substitute {

namespace test {

void test() {
	math::random::Generator rng;
	sampler::Random sampler(rng, 0);

	std::cout << "substitute::test()" << std::endl;

	Sample sample;

	math::float3 color(1.f, 1.f, 1.f);
	sample.set(color, math::float3::identity, 1.f, 0.f, 0.f, 100.f);

	math::float3 t(1.f, 0.f, 0.f);
	math::float3 b(0.f, 1.f, 0.f);
	math::float3 n(0.f, 0.f, 1.f);

	math::float3 wo = n;

	sample.set_basis(t, b, n, n, wo);

	math::float3 wi = -n;
	float pdf;
	math::float3 reflection = sample.evaluate(wi, pdf);
	print(reflection, pdf);


	BxDF_result result;
	sample.sample_evaluate(sampler, result);

	print(result);
}

}

}}}