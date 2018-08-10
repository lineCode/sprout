#include "metallic_paint_material.hpp"
#include "base/math/vector4.inl"
#include "image/texture/texture_adapter.inl"
#include "metallic_paint_sample.hpp"
#include "scene/material/coating/coating.inl"
#include "scene/material/material_sample.inl"
#include "scene/scene_renderstate.hpp"
#include "scene/scene_worker.inl"

namespace scene::material::metallic_paint {

Material::Material(Sampler_settings const& sampler_settings, bool two_sided) noexcept
    : material::Material(sampler_settings, two_sided) {}

material::Sample const& Material::sample(float3 const& wo, Renderstate const& rs,
                                         Sampler_filter filter, sampler::Sampler& /*sampler*/,
                                         Worker const& worker, uint32_t depth) const noexcept {
    auto& sample = worker.sample<Sample>(depth);

    sample.set_basis(rs.geo_n, wo);
    /*
            if (normal_map_) {
                    auto& sampler = worker.sampler_2D(sampler_key_, filter);

                    float3 nm = sampler.sample_3(*normal_map_, rs.uv);
                    float3 n  = math::normalize(rs.tangent_to_world(nm));
                    sample.layer_.set_tangent_frame(rs.t, rs.b, n);
            } else {*/
    sample.base_.set_tangent_frame(rs.t, rs.b, rs.n);

    sample.coating_.set_tangent_frame(rs.t, rs.b, rs.n);
    //	}

    auto& sampler = worker.sampler_2D(sampler_key(), filter);

    if (flakes_normal_map_.is_valid()) {
        float3 nm = flakes_normal_map_.sample_3(sampler, rs.uv);
        float3 n  = math::normalize(rs.tangent_to_world(nm));

        sample.flakes_.set_tangent_frame(n);
    } else {
        sample.flakes_.set_tangent_frame(rs.t, rs.b, rs.n);
    }

    sample.base_.set(color_a_, color_b_, alpha_);

    float flakes_weight;
    if (flakes_mask_.is_valid()) {
        flakes_weight = flakes_mask_.sample_1(sampler, rs.uv);
    } else {
        flakes_weight = 1.f;
    }

    sample.flakes_.set(flakes_ior_, flakes_absorption_, flakes_alpha_, flakes_weight);

    sample.coating_.set_color_and_weight(coating_.color_, coating_.weight_);

    sample.coating_.set(coating_.f0_, coating_.alpha_, coating_.alpha2_);

    sample.avoid_caustics_ = rs.avoid_caustics;

    return sample;
}

float Material::ior() const noexcept {
    return ior_;
}

size_t Material::num_bytes() const noexcept {
    return sizeof(*this);
}

void Material::set_color(float3 const& a, float3 const& b) noexcept {
    color_a_ = a;
    color_b_ = b;
}

void Material::set_roughness(float roughness) noexcept {
    float const r = ggx::clamp_roughness(roughness);

    alpha_ = r * r;
}

void Material::set_flakes_mask(Texture_adapter const& mask) noexcept {
    flakes_mask_ = mask;
}

void Material::set_flakes_normal_map(Texture_adapter const& normal_map) noexcept {
    flakes_normal_map_ = normal_map;
}

void Material::set_flakes_ior(float3 const& ior) noexcept {
    flakes_ior_ = ior;
}

void Material::set_flakes_absorption(float3 const& absorption) noexcept {
    flakes_absorption_ = absorption;
}

void Material::set_flakes_roughness(float roughness) noexcept {
    float const r = ggx::clamp_roughness(roughness);

    flakes_alpha_ = r * r;
}

void Material::set_coating_weight(float weight) noexcept {
    coating_.weight_ = weight;
}

void Material::set_coating_color(float3 const& color) noexcept {
    coating_.color_ = color;
}

void Material::set_clearcoat(float ior, float roughness) noexcept {
    ior_         = ior;
    coating_.f0_ = fresnel::schlick_f0(1.f, ior);

    float const r     = ggx::clamp_roughness(roughness);
    float const alpha = r * r;

    coating_.alpha_  = alpha;
    coating_.alpha2_ = alpha * alpha;
}

size_t Material::sample_size() noexcept {
    return sizeof(Sample);
}

}  // namespace scene::material::metallic_paint
