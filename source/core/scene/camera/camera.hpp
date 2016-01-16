#pragma once

#include "scene/entity/entity.hpp"
#include "base/math/rectangle.hpp"

namespace sampler { struct Camera_sample; }

namespace rendering {

class Worker;

namespace sensor { class Sensor; }

}

namespace scene {

struct Ray;

namespace camera {

class Camera : public entity::Entity {
public:

	Camera(math::int2 resolution, float ray_max_t, float frame_duration, bool motion_blur);

	virtual ~Camera();

	virtual uint32_t num_views() const = 0;

	virtual math::int2 sensor_dimensions() const = 0;

	virtual math::Recti view_bounds(uint32_t view) const = 0;

	virtual void update_focus(rendering::Worker& worker) = 0;

	virtual void generate_ray(const sampler::Camera_sample& sample, uint32_t view, scene::Ray& ray) const = 0;

	math::int2 resolution() const;

	rendering::sensor::Sensor& sensor() const;
	void set_sensor(rendering::sensor::Sensor* sensor);

	math::uint2 seed(math::int2 pixel) const;
	void set_seed(math::int2 pixel, math::uint2 seed);

	float frame_duration() const;

	bool motion_blur() const;

protected:

	virtual void on_set_transformation() final override;

	math::int2 resolution_;
	rendering::sensor::Sensor* sensor_;

	math::int2 seed_dimensions_;
	math::uint2* seeds_;
	int32_t filter_radius_;

	float ray_max_t_;
	float frame_duration_;
	bool motion_blur_;
};

}}
