#include "light_emissionmap_animated.hpp"
#include "base/math/distribution/distribution_2d.inl"
#include "base/math/math.hpp"
#include "base/math/vector4.inl"
#include "base/spectrum/rgb.hpp"
#include "image/texture/texture_adapter.inl"
#include "light_material_sample.hpp"
#include "scene/material/material_sample.inl"
#include "scene/scene_constants.hpp"
#include "scene/scene_renderstate.hpp"
#include "scene/scene_worker.inl"
#include "scene/shape/shape.hpp"

namespace scene::material::light {

Emissionmap_animated::Emissionmap_animated(Sampler_settings const& sampler_settings, bool two_sided,
                                           Texture_adapter const& emission_map,
                                           float                  emission_factor,
                                           uint64_t               animation_duration) noexcept
    : Material(sampler_settings, two_sided),
      emission_map_(emission_map),
      average_emission_(-1.f),
      emission_factor_(emission_factor),
      frame_length_(animation_duration /
                    static_cast<uint64_t>(emission_map.texture().num_elements())),
      element_(-1) {}

Emissionmap_animated::~Emissionmap_animated() noexcept {}

void Emissionmap_animated::tick(float absolute_time, float /*time_slice*/) noexcept {
    int32_t const element = static_cast<int32_t>(absolute_time / frame_length_) %
                            emission_map_.texture().num_elements();

    if (element != element_) {
        element_          = element;
        average_emission_ = float3(-1.f);
    }
}

material::Sample const& Emissionmap_animated::sample(float3 const& wo, Renderstate const& rs,
                                                     Filter filter, sampler::Sampler& /*sampler*/,
                                                     Worker const& worker, uint32_t depth) const
    noexcept {
    auto& sample = worker.sample<Sample>(depth);

    auto& sampler = worker.sampler_2D(sampler_key(), filter);

    sample.set_basis(rs.geo_n, wo);

    sample.layer_.set_tangent_frame(rs.t, rs.b, rs.n);

    float3 const radiance = emission_map_.sample_3(sampler, rs.uv, element_);

    sample.set(emission_factor_ * radiance);

    return sample;
}

float3 Emissionmap_animated::evaluate_radiance(float3 const& /*wi*/, float2 uv, float /*area*/,
                                               Filter filter, Worker const& worker) const noexcept {
    auto& sampler = worker.sampler_2D(sampler_key(), filter);
    return emission_factor_ * emission_map_.sample_3(sampler, uv, element_);
}

float3 Emissionmap_animated::average_radiance(float /*area*/) const noexcept {
    return average_emission_;
}

float Emissionmap_animated::ior() const noexcept {
    return 1.5f;
}

bool Emissionmap_animated::has_emission_map() const noexcept {
    return emission_map_.is_valid();
}

Material::Sample_2D Emissionmap_animated::radiance_sample(float2 r2) const noexcept {
    auto const result = distribution_.sample_continuous(r2);

    return {result.uv, result.pdf * total_weight_};
}

float Emissionmap_animated::emission_pdf(float2 uv, Filter filter, Worker const& worker) const
    noexcept {
    auto& sampler = worker.sampler_2D(sampler_key(), filter);

    return distribution_.pdf(sampler.address(uv)) * total_weight_;
}

float Emissionmap_animated::opacity(float2 uv, uint64_t /*time*/, Filter filter,
                                    Worker const& worker) const noexcept {
    if (mask_.is_valid()) {
        auto& sampler = worker.sampler_2D(sampler_key(), filter);
        return mask_.sample_1(sampler, uv, element_);
    } else {
        return 1.f;
    }
}

void Emissionmap_animated::prepare_sampling(shape::Shape const& shape, uint32_t /*part*/,
                                            uint64_t time, Transformation const& /*transformation*/,
                                            float /*area*/, bool importance_sampling,
                                            thread::Pool& pool) noexcept {
    int32_t const element = static_cast<int32_t>(
        (time / frame_length_) % static_cast<uint64_t>(emission_map_.texture().num_elements()));

    if (element == element_) {
        return;
    }

    element_ = element;

    if (importance_sampling) {
        auto const& texture = emission_map_.texture();

        auto const d = texture.dimensions_2();

        std::vector<math::Distribution_2D::Distribution_impl> conditional(
            static_cast<uint32_t>(d[1]));

        std::vector<float4> artws(pool.num_threads(), float4::identity());

        float2 const rd(1.f / static_cast<float>(d[0]), 1.f / static_cast<float>(d[1]));

        float const ef = emission_factor_;

        pool.run_range(
            [&conditional, &artws, &shape, &texture, d, rd, element, ef](uint32_t id, int32_t begin,
                                                                         int32_t end) {
                std::vector<float> luminance(static_cast<uint32_t>(d[0]));

                float4 artw(0.f);

                for (int32_t y = begin; y < end; ++y) {
                    float const v = rd[1] * (static_cast<float>(y) + 0.5f);

                    for (int32_t x = 0; x < d[0]; ++x) {
                        float const u = rd[0] * (static_cast<float>(x) + 0.5f);

                        float const uv_weight = shape.uv_weight(float2(u, v));

                        float3 const radiance = ef * texture.at_element_3(x, y, element);

                        luminance[static_cast<uint32_t>(x)] = uv_weight *
                                                              spectrum::luminance(radiance);

                        artw += float4(uv_weight * radiance, uv_weight);
                    }

                    conditional[static_cast<uint32_t>(y)].init(luminance.data(),
                                                               static_cast<uint32_t>(d[0]));
                }

                artws[id] += artw;
            },
            0, d[1]);

        // arw: (float3(averave_radiance), total_weight)
        float4 artw(0.f);
        for (auto& a : artws) {
            artw += a;
        }

        average_emission_ = artw.xyz() / artw[3];

        total_weight_ = artw[3];

        distribution_.init(conditional);
    } else {
        average_emission_ = emission_factor_ * emission_map_.texture().average_3();
    }

    if (is_two_sided()) {
        average_emission_ *= 2.f;
    }
}

bool Emissionmap_animated::is_animated() const noexcept {
    return true;
}

size_t Emissionmap_animated::num_bytes() const noexcept {
    return sizeof(*this);
}

}  // namespace scene::material::light
