#pragma once

#include "camera.hpp"

namespace scene { namespace camera {

class Hemispherical : public Camera {
public:

	Hemispherical(math::int2 resolution, float ray_max_t, float frame_duration, bool motion_blur);

	virtual uint32_t num_views() const final override;

	virtual math::int2 sensor_dimensions() const final override;

	virtual math::Recti view_bounds(uint32_t view) const final override;

	virtual void update_focus(rendering::Worker& worker) final override;

	virtual bool generate_ray(const sampler::Camera_sample& sample, uint32_t view,
							  scene::Ray& ray) const final override;

private:

	float d_x_;
	float d_y_;
};

}}