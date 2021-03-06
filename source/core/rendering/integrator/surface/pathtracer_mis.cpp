#include "pathtracer_mis.hpp"
#include "base/math/vector4.inl"
#include "base/memory/align.hpp"
#include "base/random/generator.inl"
#include "base/spectrum/rgb.hpp"
#include "rendering/integrator/integrator_helper.hpp"
#include "rendering/rendering_worker.hpp"
#include "scene/light/light.hpp"
#include "scene/material/bxdf.hpp"
#include "scene/material/material.hpp"
#include "scene/material/material_sample.inl"
#include "scene/prop/interface_stack.inl"
#include "scene/prop/prop_intersection.inl"
#include "scene/scene.hpp"
#include "scene/scene_constants.hpp"
#include "scene/scene_ray.inl"
#include "scene/shape/shape_sample.hpp"

#include "base/debug/assert.hpp"

namespace rendering::integrator::surface {

using namespace scene;

Pathtracer_MIS::Pathtracer_MIS(rnd::Generator& rng, take::Settings const& take_settings,
                               Settings const& settings) noexcept
    : Integrator(rng, take_settings),
      settings_(settings),
      sampler_(rng),
      material_samplers_{rng, rng, rng},
      light_samplers_{rng, rng, rng} {}

Pathtracer_MIS::~Pathtracer_MIS() {}

void Pathtracer_MIS::prepare(Scene const& scene, uint32_t num_samples_per_pixel) noexcept {
    uint32_t const num_lights = static_cast<uint32_t>(scene.lights().size());

    sampler_.resize(num_samples_per_pixel, settings_.num_samples, 1, 1);

    for (auto& s : material_samplers_) {
        s.resize(num_samples_per_pixel, settings_.num_samples, 1, 1);
    }

    uint32_t const num_light_samples = settings_.num_samples * settings_.light_sampling.num_samples;

    if (Light_sampling::Strategy::Single == settings_.light_sampling.strategy) {
        for (auto& s : light_samplers_) {
            s.resize(num_samples_per_pixel, num_light_samples, 1, 2);
        }
    } else {
        for (auto& s : light_samplers_) {
            s.resize(num_samples_per_pixel, num_light_samples, num_lights, num_lights);
        }
    }
}

void Pathtracer_MIS::start_pixel() noexcept {
    sampler_.start_pixel();

    for (auto& s : material_samplers_) {
        s.start_pixel();
    }

    for (auto& s : light_samplers_) {
        s.start_pixel();
    }
}

float3 Pathtracer_MIS::li(Ray& ray, Intersection& intersection, Worker& worker,
                          Interface_stack const& initial_stack) noexcept {
    float3 li(0.f);
    float3 photon_li(0.f);

    bool split_photon = false;

    for (uint32_t i = settings_.num_samples; i > 0; --i) {
        worker.reset_interface_stack(initial_stack);

        Ray split_ray = ray;

        Intersection split_intersection = intersection;

        bool const integrate_photons = settings_.num_samples == i;

        Result const result = integrate(split_ray, split_intersection, worker, integrate_photons);

        li += result.li;

        photon_li += result.photon_li;

        split_photon |= result.split_photon;
    }

    float const num_samples_reciprocal = 1.f / static_cast<float>(settings_.num_samples);

    if (split_photon) {
        return num_samples_reciprocal * (li + photon_li);
    }

    return num_samples_reciprocal * li + photon_li;
}

size_t Pathtracer_MIS::num_bytes() const noexcept {
    size_t sampler_bytes = 0;

    for (auto const& s : material_samplers_) {
        sampler_bytes += s.num_bytes();
    }

    for (auto const& s : light_samplers_) {
        sampler_bytes += s.num_bytes();
    }

    return sizeof(*this) + sampler_.num_bytes() + sampler_bytes;
}

Pathtracer_MIS::Result Pathtracer_MIS::integrate(Ray& ray, Intersection& intersection,
                                                 Worker& worker, bool integrate_photons) noexcept {
    uint32_t const max_bounces = settings_.max_bounces;

    Filter filter = Filter::Undefined;

    Bxdf_sample sample_result;

    bool primary_ray       = true;
    bool treat_as_singular = true;
    bool is_translucent    = false;
    bool evaluate_back     = true;

    float3 throughput(1.f);

    Result result{float3(0.f), float3(0.f), false};

    for (uint32_t i = ray.depth;; ++i) {
        float3 const wo = -ray.direction;

        bool const avoid_caustics = settings_.avoid_caustics && !primary_ray &&
                                    worker.interface_stack().top_is_vacuum_or_not_scattering();

        auto const& material_sample = intersection.sample(wo, ray, filter, avoid_caustics, sampler_,
                                                          worker);

        bool const same_side = material_sample.same_hemisphere(wo);

        // Only check direct eye-light connections for the very first hit.
        // Subsequent hits are handled by the MIS scheme.
        if (0 == i && same_side) {
            result.li += material_sample.radiance();
        }

        if (material_sample.is_pure_emissive()) {
            return result;
        }

        evaluate_back = material_sample.do_evaluate_back(evaluate_back, same_side);

        float const ray_offset = take_settings_.ray_offset_factor * intersection.geo.epsilon;

        result.li += throughput * sample_lights(ray, ray_offset, intersection, material_sample,
                                                evaluate_back, filter, worker);

        SOFT_ASSERT(math::all_finite(result.li));

        float const previous_bxdf_pdf = sample_result.pdf;

        // Material BSDF importance sample
        material_sample.sample(material_sampler(ray.depth), sample_result);
        if (0.f == sample_result.pdf) {
            break;
        }

        if (sample_result.type.test(Bxdf_type::Caustic)) {
            if (material_sample.ior_greater_one()) {
                if (avoid_caustics) {
                    break;
                }

                treat_as_singular = sample_result.type.test(Bxdf_type::Specular);
            }
        } else {
            treat_as_singular = false;

            if (primary_ray) {
                primary_ray = false;
                filter      = Filter::Nearest;

                if (integrate_photons || 0 != ray.depth) {
                    result.photon_li    = worker.photon_li(intersection, material_sample);
                    result.split_photon = 0 != ray.depth;
                }
            }
        }

        if (0.f == ray.wavelength) {
            ray.wavelength = sample_result.wavelength;
        }

        if (material_sample.ior_greater_one()) {
            throughput *= sample_result.reflection / sample_result.pdf;

            ray.origin = intersection.geo.p;
            ray.set_direction(sample_result.wi);
            ray.min_t = ray_offset;
            ++ray.depth;
        } else {
            ray.min_t = ray.max_t + ray_offset;
        }

        ray.max_t = scene::Ray_max_t;

        if (sample_result.type.test(Bxdf_type::Transmission)) {
            worker.interface_change(sample_result.wi, intersection);
        }

        if (!worker.interface_stack().empty()) {
            float3     vli, vtr;
            bool const hit = worker.volume(ray, intersection, filter, vli, vtr);

            result.li += throughput * vli;
            throughput *= vtr;

            if (!hit) {
                break;
            }
        } else if (!worker.intersect_and_resolve_mask(ray, intersection, filter)) {
            break;
        }

        SOFT_ASSERT(math::all_finite(result.li));

        if (!material_sample.ior_greater_one() && !treat_as_singular) {
            sample_result.pdf = previous_bxdf_pdf;
        } else {
            is_translucent = material_sample.is_translucent();
        }

        if (evaluate_back || treat_as_singular) {
            bool         pure_emissive;
            float3 const radiance = evaluate_light(ray, intersection, sample_result,
                                                   treat_as_singular, is_translucent, filter,
                                                   worker, pure_emissive);

            result.li += throughput * radiance;

            if (pure_emissive) {
                break;
            }
        }

        if (ray.depth >= max_bounces) {
            break;
        }

        if (ray.depth > settings_.min_bounces) {
            float const q = settings_.path_continuation_probability;
            if (rendering::russian_roulette(throughput, q, sampler_.generate_sample_1D())) {
                break;
            }
        }
    }

    return result;
}

float3 Pathtracer_MIS::sample_lights(Ray const& ray, float ray_offset, Intersection& intersection,
                                     const Material_sample& material_sample, bool evaluate_back,
                                     Filter filter, Worker& worker) noexcept {
    float3 result(0.f);

    if (!material_sample.ior_greater_one()) {
        return result;
    }

    uint32_t const num_samples = settings_.light_sampling.num_samples;

    float const num_light_samples_reciprocal = 1.f / static_cast<float>(num_samples);

    if (Light_sampling::Strategy::Single == settings_.light_sampling.strategy) {
        for (uint32_t i = num_samples; i > 0; --i) {
            float const select = light_sampler(ray.depth).generate_sample_1D(1);

            auto const light = worker.scene().random_light(select);

            float3 const el = evaluate_light(light.ref, light.pdf, ray, ray_offset, 0,
                                             evaluate_back, intersection, material_sample, filter,
                                             worker);

            result += num_light_samples_reciprocal * el;
        }
    } else {
        auto const& lights = worker.scene().lights();
        for (uint32_t l = 0, len = static_cast<uint32_t>(lights.size()); l < len; ++l) {
            auto const& light = *lights[l];
            for (uint32_t i = num_samples; i > 0; --i) {
                float3 const el = evaluate_light(light, 1.f, ray, ray_offset, l, evaluate_back,
                                                 intersection, material_sample, filter, worker);

                result += num_light_samples_reciprocal * el;
            }
        }
    }

    return result;
}

float3 Pathtracer_MIS::evaluate_light(const Light& light, float light_weight, Ray const& history,
                                      float ray_offset, uint32_t sampler_dimension,
                                      bool evaluate_back, Intersection const& intersection,
                                      const Material_sample& material_sample, Filter filter,
                                      Worker& worker) noexcept {
    // Light source importance sample
    shape::Sample_to light_sample;
    if (!light.sample(intersection.geo.p, material_sample.geometric_normal(), history.time,
                      material_sample.is_translucent(), light_sampler(history.depth),
                      sampler_dimension, worker, light_sample)) {
        return float3(0.f);
    }

    float const shadow_offset = take_settings_.ray_offset_factor * light_sample.epsilon;

    Ray shadow_ray(intersection.geo.p, light_sample.wi, ray_offset, light_sample.t - shadow_offset,
                   history.depth, history.time, history.wavelength);

    float3 tv;
    if (!worker.transmitted_visibility(shadow_ray, intersection, filter, tv)) {
        return float3(0.f);
    }

    SOFT_ASSERT(math::all_finite(tv));

    auto const bxdf = material_sample.evaluate(light_sample.wi, evaluate_back);

    float3 const radiance = light.evaluate(light_sample, Filter::Nearest, worker);

    float const light_pdf = light_sample.pdf * light_weight;
    float const weight    = evaluate_back ? power_heuristic(light_pdf, bxdf.pdf) : 1.f;

    return (weight / light_pdf) * (tv * radiance * bxdf.reflection);
}

float3 Pathtracer_MIS::evaluate_light(Ray const& ray, Intersection const& intersection,
                                      Bxdf_sample sample_result, bool treat_as_singular,
                                      bool is_translucent, Filter filter, Worker& worker,
                                      bool& pure_emissive) noexcept {
    uint32_t const light_id = intersection.light_id();
    if (!Light::is_light(light_id)) {
        pure_emissive = false;
        return float3(0.f);
    }

    float light_pdf = 0.f;

    if (!treat_as_singular) {
        bool const calculate_pdf = Light_sampling::Strategy::Single ==
                                   settings_.light_sampling.strategy;

        auto const light = worker.scene().light(light_id, calculate_pdf);

        float const ls_pdf = light.ref.pdf(ray, intersection.geo, is_translucent, Filter::Nearest,
                                           worker);

        if (0.f == ls_pdf) {
            pure_emissive = true;
            return float3(0.f);
        }

        light_pdf = ls_pdf * light.pdf;
    }

    float3 const wo = -sample_result.wi;

    // This will invalidate the contents of previous previous material samples.
    auto const& light_material_sample = intersection.sample(wo, ray, filter, false, sampler_,
                                                            worker);

    pure_emissive = light_material_sample.is_pure_emissive();

    if (!light_material_sample.same_hemisphere(wo)) {
        return float3(0.f);
    }

    float3 const ls_energy = light_material_sample.radiance();

    float const weight = power_heuristic(sample_result.pdf, light_pdf);

    float3 const radiance = weight * ls_energy;

    SOFT_ASSERT(math::all_finite_and_positive(radiance));

    return radiance;
}

sampler::Sampler& Pathtracer_MIS::material_sampler(uint32_t bounce) noexcept {
    if (Num_material_samplers > bounce) {
        return material_samplers_[bounce];
    }

    return sampler_;
}

sampler::Sampler& Pathtracer_MIS::light_sampler(uint32_t bounce) noexcept {
    if (Num_light_samplers > bounce) {
        return light_samplers_[bounce];
    }

    return sampler_;
}

Pathtracer_MIS_factory::Pathtracer_MIS_factory(take::Settings const& take_settings,
                                               uint32_t num_integrators, uint32_t num_samples,
                                               uint32_t min_bounces, uint32_t max_bounces,
                                               float          path_termination_probability,
                                               Light_sampling light_sampling,
                                               bool           enable_caustics) noexcept
    : Factory(take_settings),
      integrators_(memory::allocate_aligned<Pathtracer_MIS>(num_integrators)),
      settings_{num_samples,    min_bounces,     max_bounces, 1.f - path_termination_probability,
                light_sampling, !enable_caustics} {}

Pathtracer_MIS_factory::~Pathtracer_MIS_factory() noexcept {
    memory::free_aligned(integrators_);
}

Integrator* Pathtracer_MIS_factory::create(uint32_t id, rnd::Generator& rng) const noexcept {
    return new (&integrators_[id]) Pathtracer_MIS(rng, take_settings_, settings_);
}

}  // namespace rendering::integrator::surface
