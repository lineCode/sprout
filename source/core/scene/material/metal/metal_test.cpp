#include "metal_test.hpp"
#include <iostream>
#include "base/math/print.hpp"
#include "base/math/vector3.inl"
#include "base/random/generator.inl"
#include "metal_presets.hpp"
#include "metal_sample.hpp"
#include "sampler/sampler_random.hpp"
#include "scene/material/ggx/ggx.inl"
#include "scene/material/material_print.hpp"
#include "scene/material/material_sample.inl"

namespace scene::material::metal::testing {

struct Setup {
    void test(float3 const& wi, float3 const& wo, float3 const& t, float3 const& b, float3 const& n,
              sampler::Sampler& sampler);

    float3 ior;
    float3 absorption;
    float  roughness = 0.01f;
};

void test() {
    rnd::Generator  rng(0, 0);
    sampler::Random sampler(rng);
    sampler.resize(0, 1, 1, 1);

    std::cout << "metal::testing::test()" << std::endl;

    Setup setup;

    ior_and_absorption("Gold", setup.ior, setup.absorption);

    float3 t(1.f, 0.f, 0.f);
    float3 b(0.f, 1.f, 0.f);
    float3 n(0.f, 0.f, 1.f);

    float3 arbitrary = math::normalize(float3(0.5f, 0.5f, 0.5f));

    float3 wo = n;
    float3 wi = arbitrary;  // n;

    setup.test(wi, wo, t, b, n, sampler);

    setup.test(t, t, t, b, n, sampler);
    setup.test(t, b, t, b, n, sampler);
    setup.test(t, n, t, b, n, sampler);
    setup.test(b, t, t, b, n, sampler);
    setup.test(b, b, t, b, n, sampler);
    setup.test(b, n, t, b, n, sampler);
    setup.test(n, t, t, b, n, sampler);
    setup.test(n, b, t, b, n, sampler);
    setup.test(n, n, t, b, n, sampler);

    /*
    Sample sample;

    float3 color(1.f, 1.f, 1.f);
    float3 radiance = float3::identity();
    float ior         = 1.47f;
    float constant_f0 = fresnel::schlick_f0(1.f, ior);
    float roughness   = 0.f;
    float metallic    = 0.f;
    sample.layer_.set(color, radiance, ior, constant_f0, roughness, metallic);

    float3 t(1.f, 0.f, 0.f);
    float3 b(0.f, 1.f, 0.f);
    float3 n(0.f, 0.f, 1.f);

    float3 arbitrary = math::normalize(float3(0.5f, 0.5f, 0.5f));

    float3 wo = n;

    sample.set_tangent_frame(n, wo);
    sample.layer_.set_tangent_frame(t, b, n);

    float3 wi = arbitrary;//n;
    float pdf;
    float3 reflection = sample.evaluate(wi, pdf);
    print(reflection, pdf);

    bxdf::Sample result;
    sample.sample(sampler, result);

    print(result);


    float xiy = 1.f;
    float alpha2 = ggx::Min_alpha2;
    float n_dot_h = std::sqrt((1.f - xiy) / ((alpha2 - 1.f) * xiy + 1.f));

    std::cout << "n_dot_h == " << n_dot_h << std::endl;

    n_dot_h = 1.f;
    float d = ggx::distribution_isotropic(n_dot_h, ggx::Min_alpha2);

    std::cout << "d == " << d << std::endl;

    float const n_dot_wo = 1.f;
    float g = ggx::geometric_visibility(n_dot_wo, n_dot_wo, ggx::Min_alpha2);
    std::cout << "g == " << g << std::endl;
    */
}

void Setup::test(float3 const& wi, float3 const& wo, float3 const& t, float3 const& b,
                 float3 const& n, sampler::Sampler& sampler) {
    Sample_isotropic sample;

    sample.set(ior, absorption, roughness, false);

    sample.set_basis(n, wo);
    sample.layer_.set_tangent_frame(t, b, n);

    {
        auto const result = sample.evaluate(wi, true);
        print(result);
    }

    bxdf::Sample result;
    sample.sample(sampler, result);

    print(result);
}

}  // namespace scene::material::metal::testing
