#pragma once

#include "camera.hpp"

namespace scene { namespace camera {

class Perspective : public Camera {
public:

	Perspective(const math::float2& dimensions, rendering::film::Film* film,
				float frame_duration, bool motion_blur,
				float fov, float lens_radius, float focal_distance,
				float ray_max_t);

	virtual void update_view() final override;

	virtual void generate_ray(const sampler::Camera_sample& sample,
							  float normalized_tick_offset, float normalized_tick_slice,
							  math::Oray& ray) const final override;

private:

	float fov_;
	float lens_radius_;
	float focal_distance_;
	float ray_max_t_;

	math::float3 left_top_;
	math::float3 d_x_, d_y_;
};

}}
