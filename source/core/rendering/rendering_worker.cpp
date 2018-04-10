#include "rendering_worker.hpp"
#include "rendering/integrator/surface/surface_integrator.hpp"
#include "rendering/integrator/volume/volume_integrator.hpp"
#include "sampler/sampler.hpp"
#include "scene/scene.hpp"
#include "scene/scene_constants.hpp"
#include "scene/scene_ray.inl"
#include "scene/material/material.hpp"
#include "scene/prop/prop.hpp"
#include "scene/prop/prop_intersection.inl"
#include "base/math/vector4.inl"
#include "base/math/sampling/sample_distribution.hpp"
#include "base/memory/align.hpp"
#include "base/spectrum/rgb.hpp"

#include "base/debug/assert.hpp"

namespace rendering {

Worker::~Worker() {
	memory::safe_destruct(sampler_);
	memory::safe_destruct(volume_integrator_);
	memory::safe_destruct(surface_integrator_);
}

void Worker::init(uint32_t id, const take::Settings& settings,
				  const scene::Scene& scene, uint32_t max_sample_size,
				  integrator::surface::Factory& surface_integrator_factory,
				  integrator::volume::Factory& volume_integrator_factory,
				  sampler::Factory& sampler_factory) {
	scene::Worker::init(id, settings, scene, max_sample_size);

	surface_integrator_ = surface_integrator_factory.create(id, rng_);
	volume_integrator_  = volume_integrator_factory.create(id, rng_);
	sampler_			= sampler_factory.create(id, rng_);
}

void Worker::prepare(uint32_t num_samples_per_pixel) {
	surface_integrator_->prepare(*scene_, num_samples_per_pixel);
	volume_integrator_->prepare(*scene_, num_samples_per_pixel);
	sampler_->resize(num_samples_per_pixel, 1, 2, 1);
}

float4 Worker::li(Ray& ray) {
	scene::prop::Intersection intersection;
	const bool hit = intersect_and_resolve_mask(ray, intersection, Sampler_filter::Undefined);

	float3 vtr(1.f);
	const float3 vli = volume_li(ray, vtr);

	SOFT_ASSERT(math::all_finite_and_positive(vli));

	if (hit) {
		const float3 li = surface_integrator_->li(ray, intersection, *this);

		SOFT_ASSERT(math::all_finite_and_positive(li));

		return float4(vtr * li + vli, 1.f);
	} else {
		return float4(vli, spectrum::luminance(vli));
	}
}

float3 Worker::li(Ray& ray, Intersection& intersection) {
	return surface_integrator_->li(ray, intersection, *this);
}

float3 Worker::volume_li(const Ray& ray, float3& transmittance) {
	float3 tr(1.f);
	float3 radiance(0.f);

	Ray tray = ray;

	for (; tray.min_t < tray.max_t;) {
		float epsilon;
		const auto volume = scene_->closest_volume_segment(tray, node_stack_, epsilon);
		if (!volume || tray.max_t >= scene::Almost_ray_max_t_minus_epsilon) {
			// By convention don't integrate infinite volumes,
			// as the result should be pre-computed in the surrounding infinite shape alredy.
			break;
		}

		// Otherwise too small to handle meaningfully, but we still want to continue raymarching
		if (tray.max_t - tray.min_t > 0.0005f) {
			float3 temp;
			radiance += tr * volume_integrator_->li(tray, *volume, *this, temp);
			tr *= temp;
		}

		SOFT_ASSERT(tray.max_t + epsilon - tray.min_t > 0.0001f);

		tray.min_t = tray.max_t + epsilon;
		tray.max_t = ray.max_t;
	}

	transmittance = tr;
	return radiance;
}

bool Worker::volume(Ray& ray, Intersection& intersection,
					float3& li, float3& transmittance, float3& weight) {
	return volume_integrator_->integrate(ray, intersection, *this, li, transmittance, weight);
}

float3 Worker::transmittance(const Ray& ray) const {
	float3 transmittance(1.f);

	Ray tray = ray;

	tray.properties.set(Ray::Property::Shadow);

	for (; tray.min_t < tray.max_t;) {
		float epsilon;
		const auto volume = scene_->closest_volume_segment(tray, node_stack_, epsilon);
		if (!volume || tray.max_t >= scene::Almost_ray_max_t_minus_epsilon) {
			// By convention don't integrate infinite volumes,
			// as the result should be pre-computed in the surrounding infinite shape alredy.
			break;
		}

		// Otherwise too small to handle meaningfully, but we still want to continue raymarching.
		if (tray.max_t - tray.min_t > 0.0005f) {
			transmittance *= volume_integrator_->transmittance(tray, *volume, *this);
		}

		SOFT_ASSERT(tray.max_t + epsilon - tray.min_t > 0.0001f);

		tray.min_t = tray.max_t + epsilon;
		tray.max_t = ray.max_t;
	}

	return transmittance;
}


float3 Worker::tinted_visibility(const Ray& ray, Sampler_filter filter) const {
	return float3(1.f) - scene_->thin_absorption(ray, filter, *this);
}

float3 Worker::tinted_visibility(Ray& ray, const Intersection& intersection,
								 Sampler_filter filter) {
	if (intersection.geo.subsurface) {
		const float ray_max_t = ray.max_t;

		float epsilon;
		if (intersect(intersection.prop, ray, epsilon)) {
			const float3 tr = volume_integrator_->transmittance(ray, intersection, *this);

			ray.min_t = ray.max_t + epsilon * settings_.ray_offset_factor;
			ray.max_t = ray_max_t;

			return tr * tinted_visibility(ray, filter);
		}
	}

	return tinted_visibility(ray, filter);
}

sampler::Sampler* Worker::sampler() {
	return sampler_;
}

}
