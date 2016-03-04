#pragma once

#include "scene_worker.hpp"
#include "animation/animation_stage.hpp"
#include "bvh/scene_bvh_builder.hpp"
#include "bvh/scene_bvh_tree.hpp"
#include "material/material.hpp"
#include "base/math/ray.hpp"
#include "base/math/distribution/distribution_1d.hpp"
#include <vector>
#include <memory>

namespace thread { class Pool; }

namespace scene {

namespace shape {

class Shape;

}

namespace entity {

class Entity;
class Dummy;

}

namespace light {

class Light;
class Prop_light;
class Prop_image_light;

}

namespace animation { class Animation; }

namespace volume { class Volume; }

struct Intersection;
struct Ray;
class Prop;

class Scene {
public:

	Scene();
	~Scene();

	const math::aabb& aabb() const;

	bool intersect(scene::Ray& ray, shape::Node_stack& node_stack, Intersection& intersection) const;
	bool intersect_p(const scene::Ray& ray, shape::Node_stack& node_stack) const;

	float opacity(const scene::Ray& ray, Worker& worker, material::Sampler_settings::Filter filter) const;

	float tick_duration() const;
	float simulation_time() const;

	const std::vector<light::Light*>& lights() const;

	const light::Light* montecarlo_light(float random, float& pdf) const;

	const volume::Volume* volume_region() const;

	void tick(thread::Pool& thread_pool);
	float seek(float time, thread::Pool& thread_pool);

	entity::Dummy* create_dummy();

	Prop* create_prop(std::shared_ptr<shape::Shape> shape, const material::Materials& materials);

	light::Prop_light* create_prop_light(Prop* prop, uint32_t part);
	light::Prop_image_light* create_prop_image_light(Prop* prop, uint32_t part);

	volume::Volume* create_volume(const math::float3& absorption, const math::float3& scattering);

	void add_material(std::shared_ptr<material::Material> material);
    void add_animation(std::shared_ptr<animation::Animation> animation);

    void create_animation_stage(entity::Entity* entity, animation::Animation* animation);

public:

	void compile();

	float tick_duration_;
	float simulation_time_;

    bvh::Builder builder_;
	bvh::Tree bvh_;

	std::vector<entity::Dummy*> dummies_;

	std::vector<Prop*> finite_props_;
	std::vector<Prop*> infinite_props_;

	std::vector<light::Light*> lights_;

	std::vector<float> light_powers_;

	math::Distribution_1D light_distribution_;

	volume::Volume* volume_region_;

	std::vector<std::shared_ptr<material::Material>> materials_;

    std::vector<std::shared_ptr<animation::Animation>> animations_;

    std::vector<animation::Stage> animation_stages_;
};

}
