#include "camera_cubic_stereoscopic.hpp"
#include "rendering/sensor/sensor.hpp"
#include "scene/scene_ray.inl"
#include "sampler/camera_sample.hpp"
#include "base/json/json.hpp"
#include "base/math/math.hpp"
#include "base/math/vector.inl"
#include "base/math/matrix.inl"
#include "base/math/ray.inl"
#include "base/math/sampling/sampling.inl"

namespace scene { namespace camera {

Cubic_stereoscopic::Cubic_stereoscopic(Layout layout, int2 resolution, float ray_max_t) :
	Stereoscopic(int2(resolution.x, resolution.x), ray_max_t) {
	float f = static_cast<float>(resolution.x);

	left_top_ = float3(-1.f, 1.f, 1.f);

	float3 right_top  ( 1.f,  1.f, 1.f);
	float3 left_bottom(-1.f, -1.f, 1.f);

	d_x_ = (right_top - left_top_)   / f;
	d_y_ = (left_bottom - left_top_) / f;

	if (Layout::lxlmxlylmylzlmzrxrmxryrmyrzrmz == layout) {
		for (uint32_t i = 0; i < 12; ++i) {
			int2 offset = int2(resolution.x * i, 0);

			view_bounds_[i] = math::Recti{offset, offset + resolution_};
		}

		sensor_dimensions_ = int2(resolution_.x * 12, resolution_.x);
	} else if (Layout::rxlmxryrmyrzrmzlxlmxlylmylzlmz == layout) {
		for (uint32_t i = 0; i < 6; ++i) {
			int2 offset = int2(resolution.x * (i + 6), 0);

			view_bounds_[i] = math::Recti{offset, offset + resolution_};
		}

		for (uint32_t i = 6; i < 12; ++i) {
			int2 offset = int2(resolution.x * (i - 6), 0);

			view_bounds_[i] = math::Recti{offset, offset + resolution_};
		}

		sensor_dimensions_ = int2(resolution_.x * 12, resolution_.x);
	}

	math::set_rotation_y(view_rotations_[0], math::degrees_to_radians(-90.f));
	math::set_rotation_y(view_rotations_[1], math::degrees_to_radians( 90.f));
	math::set_rotation_x(view_rotations_[2], math::degrees_to_radians( 90.f));
	math::set_rotation_x(view_rotations_[3], math::degrees_to_radians(-90.f));
	view_rotations_[4] = math::float3x3::identity();
	math::set_rotation_y(view_rotations_[5], math::degrees_to_radians(180.f));

	set_interpupillary_distance_falloff(0.36f);
}

uint32_t Cubic_stereoscopic::num_views() const {
	return 12;
}

int2 Cubic_stereoscopic::sensor_dimensions() const {
	return sensor_dimensions_;
}

math::Recti Cubic_stereoscopic::view_bounds(uint32_t view) const {
	return view_bounds_[view];
}

void Cubic_stereoscopic::update_focus(rendering::Worker& /*worker*/) {}

bool Cubic_stereoscopic::generate_ray(const sampler::Camera_sample& sample, uint32_t view,
									  scene::Ray& ray) const {
	float2 coordinates = float2(sample.pixel) + sample.pixel_uv;

	float3 direction = left_top_ + coordinates.x * d_x_ + coordinates.y * d_y_;

	uint32_t face = view % 6;
	direction = math::normalized(direction * view_rotations_[face]);

	float a = -std::atan2(direction.x, direction.z);

	math::float3x3 rotation;
	math::set_rotation_y(rotation, a);

	float ipd_scale = 1.f - std::pow(std::abs(direction.y), 12.f - ipd_falloff_ * 10.f);

	uint32_t eye = view < 6 ? 0 : 1;
	float3 eye_offset = (ipd_scale * eye_offsets_[eye]) * rotation;

	entity::Composed_transformation temp;
	auto& transformation = transformation_at(sample.time, temp);

	ray.origin = math::transform_point(eye_offset, transformation.object_to_world);
	ray.set_direction(math::transform_vector(direction, transformation.object_to_world));
	ray.min_t = 0.f;
	ray.max_t = ray_max_t_;
	ray.time  = sample.time;
	ray.depth = 0;

	return true;
}

void Cubic_stereoscopic::set_interpupillary_distance_falloff(float ipd_falloff) {
	ipd_falloff_ = std::sqrt(ipd_falloff);
}

void Cubic_stereoscopic::set_parameter(const std::string& name,
									   const json::Value& value) {
	if ("stereo" == name) {
		for (auto n = value.MemberBegin(); n != value.MemberEnd(); ++n) {
			const std::string node_name = n->name.GetString();
			const json::Value& node_value = n->value;

			if ("ipd" == node_name) {
				set_interpupillary_distance(json::read_float(node_value));
			} else if ("ipd_falloff" == node_name) {
				set_interpupillary_distance_falloff(json::read_float(node_value));
			}
		}
	}
}

}}
