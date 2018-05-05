#include "volumetric_grid.hpp"
#include "image/texture/texture_adapter.inl"
#include "scene/scene_worker.hpp"
#include "scene/entity/composed_transformation.hpp"
#include "base/math/matrix4x4.inl"
#include "base/math/ray.inl"
#include "base/random/generator.inl"
#include "base/spectrum/heatmap.hpp"

#include <iostream>

namespace scene::material::volumetric {

Grid::Grid(const Sampler_settings& sampler_settings, const Texture_adapter& grid) :
	Density(sampler_settings), grid_(grid) {
}

Grid::~Grid() {}

void Grid::compile() {
	float max_density = 0.f;

	auto const texture = grid_.texture();

	const int3 d = texture->dimensions_3();

	for (int32_t i = 0, len = d[0] * d[1] * d[2]; i < len; ++i) {
		max_density = std::max(texture->at_1(i), max_density);
	}

	float3 const extinction_coefficient = absorption_coefficient_ + scattering_coefficient_;

	majorant_mu_t_ = max_density * math::max_component(extinction_coefficient);
}

float Grid::majorant_mu_t() const {
	return majorant_mu_t_;
}

bool Grid::is_heterogeneous_volume() const {
	return true;
}

size_t Grid::num_bytes() const {
	return sizeof(*this);
}

float Grid::density(f_float3 p, Sampler_filter filter, const Worker& worker) const {
	// p is in object space already

	float3 p_g = 0.5f * (float3(1.f) + p);
	p_g[1] = 1.f - p_g[1];

	auto const& sampler = worker.sampler_3D(sampler_key(), filter);

	return grid_.sample_1(sampler, p_g);
}

Emission_grid::Emission_grid(const Sampler_settings& sampler_settings,
							 const Texture_adapter& grid) :
	Material(sampler_settings), grid_(grid) {}

Emission_grid::~Emission_grid() {}

float3 Emission_grid::emission(const math::Ray& ray,Transformation const& transformation,
							   float step_size, rnd::Generator& rng,
							   Sampler_filter filter, const Worker& worker) const {
	const math::Ray rn = ray.normalized();

	float min_t = rn.min_t + rng.random_float() * step_size;

	float3 emission(0.f);

	float3 const rp_o = math::transform_point(rn.origin, transformation.world_to_object);
	float3 const rd_o = math::transform_vector(rn.direction, transformation.world_to_object);

	for (; min_t < rn.max_t; min_t += step_size) {
		float3 const p_o = rp_o + min_t * rd_o; // r_o.point(min_t);
		emission += Emission_grid::emission(p_o, filter, worker);
	}

	return step_size * emission;
}

size_t Emission_grid::num_bytes() const {
	return sizeof(*this);
}

float3 Emission_grid::emission(f_float3 p, Sampler_filter filter, const Worker& worker) const {
	// p is in object space already

	float3 p_g = 0.5f * (float3(1.f) + p);
	p_g[1] = 1.f - p_g[1];

	auto const& sampler = worker.sampler_3D(sampler_key(), filter);

	return 0.00001f * grid_.sample_3(sampler, p_g);
}

Flow_vis_grid::Flow_vis_grid(const Sampler_settings& sampler_settings,
							 const Texture_adapter& grid) :
	Material(sampler_settings), grid_(grid) {}

Flow_vis_grid::~Flow_vis_grid() {}

float3 Flow_vis_grid::emission(const math::Ray& ray,Transformation const& transformation,
							   float step_size, rnd::Generator& rng,
							   Sampler_filter filter, const Worker& worker) const {
	const math::Ray rn = ray.normalized();

	float min_t = rn.min_t + rng.random_float() * step_size;

	float3 emission(0.f);

	float3 const rp_o = math::transform_point(rn.origin, transformation.world_to_object);
	float3 const rd_o = math::transform_vector(rn.direction, transformation.world_to_object);

	for (; min_t < rn.max_t; min_t += step_size) {
		float3 const p_o = rp_o + min_t * rd_o; // r_o.point(min_t);
		emission += Flow_vis_grid::emission(p_o, filter, worker);
	}

	return step_size * 8.f * emission;

/*
	float density = 0.f;

	float3 const rp_o = math::transform_point(rn.origin, transformation.world_to_object);
	float3 const rd_o = math::transform_vector(rn.direction, transformation.world_to_object);

	for (; min_t < rn.max_t; min_t += step_size) {
		float3 const p_o = rp_o + min_t * rd_o; // r_o.point(min_t);
		density += Flow_vis_grid::density(p_o, filter, worker);
	}

	density *= std::min(48.f * step_size, 1.f);

	return spectrum::heatmap(density);
	*/
}

size_t Flow_vis_grid::num_bytes() const {
	return sizeof(*this);
}

float Flow_vis_grid::density(f_float3 p, Sampler_filter filter, const Worker& worker) const {
	// p is in object space already

	float3 p_g = 0.5f * (float3(1.f) + p);
	p_g[1] = 1.f - p_g[1];

	auto const& sampler = worker.sampler_3D(sampler_key(), filter);

	return grid_.sample_1(sampler, p_g);
}

float3 Flow_vis_grid::emission(f_float3 p, Sampler_filter filter, const Worker& worker) const {
	// p is in object space already

	float3 p_g = 0.5f * (float3(1.f) + p);
	p_g[1] = 1.f - p_g[1];

	auto const& sampler = worker.sampler_3D(sampler_key(), filter);

	float const density = std::min(2.f * grid_.sample_1(sampler, p_g), 1.f);

	return spectrum::heatmap(density);
}

}
