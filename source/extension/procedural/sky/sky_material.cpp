#include "sky_material.hpp"
#include "sky_model.hpp"
#include "core/scene/scene_worker.hpp"
#include "core/scene/material/material_sample.inl"
#include "core/scene/material/material_sample_cache.inl"
#include "core/scene/shape/geometry/hitpoint.inl"
#include "base/math/vector.inl"

namespace procedural { namespace sky {

Material::Material(scene::material::Generic_sample_cache<scene::material::light::Sample>& cache,
				   Model& model) :
	scene::material::Typed_material<
		scene::material::Generic_sample_cache<scene::material::light::Sample>>(
			cache, nullptr, scene::material::Sampler_settings(), false),
	model_(model) {}

bool Material::has_emission_map() const {
	return false;
}

void Material::prepare_sampling(bool /*spherical*/) {
	model_.init();
}

Sky_material::Sky_material(
		scene::material::Generic_sample_cache<scene::material::light::Sample>& cache,
		Model& model) : Material(cache, model) {}

const scene::material::Sample& Sky_material::sample(const scene::shape::Hitpoint& hp,
													float3_p wo, float /*area*/,
													float /*time*/, float /*ior_i*/,
													const scene::Worker& worker,
													Sampler_filter /*filter*/) {
	auto& sample = cache_.get(worker.id());

	sample.set_basis(hp.geo_n, wo);

	sample.layer_.set_basis(hp.t, hp.b, hp.n, 1.f);

	sample.layer_.set(model_.evaluate_sky(-wo));

	return sample;
}

float3 Sky_material::sample_radiance(float3_p wi, float2 /*uv*/,
									 float /*area*/, float /*time*/,
									 const scene::Worker& /*worker*/,
									 Sampler_filter /*filter*/) const {
	return model_.evaluate_sky(wi);
}

float3 Sky_material::average_radiance(float /*area*/) const {
	return model_.evaluate_sky(model_.zenith());
}

Sun_material::Sun_material(
		scene::material::Generic_sample_cache<scene::material::light::Sample>& cache,
		Model& model) : Material(cache, model) {}

const scene::material::Sample& Sun_material::sample(const scene::shape::Hitpoint& hp,
													float3_p wo, float /*area*/,
													float /*time*/, float /*ior_i*/,
													const scene::Worker& worker,
													Sampler_filter /*filter*/) {
	auto& sample = cache_.get(worker.id());

	sample.set_basis(hp.geo_n, wo);

	sample.layer_.set_basis(hp.t, hp.b, hp.n, 1.f);

	sample.layer_.set(model_.evaluate_sky_and_sun(-wo));

	return sample;
}

float3 Sun_material::sample_radiance(float3_p wi, float2 /*uv*/,
									 float /*area*/, float /*time*/,
									 const scene::Worker& /*worker*/,
									 Sampler_filter /*filter*/) const {
	return model_.evaluate_sky_and_sun(wi);
}

float3 Sun_material::average_radiance(float /*area*/) const {
	return model_.evaluate_sky_and_sun(-model_.sun_direction());
}

}}
