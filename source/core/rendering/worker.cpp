#include "worker.hpp"
#include "rendering/film/film.hpp"
#include "rendering/integrator/integrator.hpp"
#include "sampler/camera_sample.hpp"
#include "sampler/sampler.hpp"
#include "scene/scene.hpp"
#include "scene/camera/camera.hpp"
#include "scene/prop/prop_intersection.hpp"
#include "scene/material/material.hpp"
#include "base/math/vector.inl"
#include "base/math/ray.inl"
#include "base/math/distribution.inl"
#include "base/math/random/generator.inl"
#include <iostream>

namespace rendering {

Worker::Worker() : sampler_(nullptr), node_stack_(128) {}

Worker::~Worker() {
	delete sampler_;
}

void Worker::init(uint32_t id, const math::random::Generator& rng, Surface_integrator_factory& surface_integrator_factory,
				  sampler::Sampler& sampler, const scene::Scene& scene) {
	id_ = id;
	rng_ = rng;
	surface_integrator_ = surface_integrator_factory.create(rng_);
	sampler_ = sampler.clone();
	scene_ = &scene;
}

uint32_t Worker::id() const {
	return id_;
}

void Worker::render(const scene::camera::Camera& camera, const Rectui& tile, uint32_t sample_start, uint32_t sample_end) {
	auto& film = camera.film();

	uint32_t num_samples = sample_end - sample_start;

	sampler::Camera_sample sample;
	math::Oray ray;

	for (uint32_t y = tile.start.y; y < tile.end.y; ++y) {
		for (uint32_t x = tile.start.x; x < tile.end.x; ++x) {

			sampler_->restart(1);
			surface_integrator_->start_new_pixel(/*sampler_->num_samples_per_iteration()*/
												 num_samples);

			math::float2 offset(static_cast<float>(x), static_cast<float>(y));

		//	while (sampler_->generate_camera_sample(offset, sample)) {
			for (uint32_t i = sample_start; i < sample_end; ++i) {
				sampler_->generate_camera_sample(offset, i, sample);

				camera.generate_ray(sample, scene_->tick_length(), ray);

				math::float3 color = li(ray);

				film.add_sample(sample, color, tile);
			}
		}
	}
}

math::float3 Worker::li(math::Oray& ray) {
	scene::Intersection intersection;
	bool hit = intersect(ray, intersection);
	if (hit) {
		return surface_integrator_->li(*this, ray, intersection);
	} else {
		return math::float3::identity;
	}
}

bool Worker::intersect(math::Oray& ray, scene::Intersection& intersection) {
	return scene_->intersect(ray, node_stack_, intersection);
}

bool Worker::visibility(const math::Oray& ray) {
	return !scene_->intersect_p(ray, node_stack_);
}

float Worker::masked_visibility(const math::Oray& ray, const image::sampler::Sampler_2D& sampler) {
	return 1.f - scene_->opacity(ray, node_stack_, sampler);
}

const scene::Scene& Worker::scene() const {
	return *scene_;
}

}
