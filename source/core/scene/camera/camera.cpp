#include "camera.hpp"
#include "base/json/json.hpp"
#include "base/math/matrix3x3.inl"
#include "base/math/matrix4x4.inl"
#include "base/math/quaternion.inl"
#include "base/math/vector3.inl"
#include "rendering/rendering_worker.hpp"
#include "rendering/sensor/sensor.hpp"
#include "scene/prop/interface_stack.inl"
#include "scene/prop/prop_intersection.inl"
#include "scene/scene.hpp"
#include "scene/scene_constants.hpp"
#include "scene/scene_ray.inl"
#include "scene/scene_worker.hpp"

namespace scene::camera {

Camera::Camera(int2 resolution) : resolution_(resolution) {}

Camera::~Camera() {}

void Camera::update(Scene const& scene, Worker& worker) {
    calculate_world_transformation();

    interface_stack_.clear();
    interfaces_.clear();

    if (scene.has_volumes()) {
        math::Transformation transformation = local_frame_a();

        Ray ray(transformation.position, float3(0.f, 1.f, 0.f), 0.f, Ray_max_t);

        prop::Intersection intersection;

        for (;;) {
            if (!scene.intersect_volume(ray, worker.node_stack(), intersection)) {
                break;
            }

            if (intersection.same_hemisphere(ray.direction)) {
                if (!interfaces_.remove(intersection)) {
                    interface_stack_.push(intersection);
                }
            } else {
                interfaces_.push(intersection);
            }

            ray.min_t = ray.max_t + intersection.geo.epsilon;  // * settings_.ray_offset_factor;
            ray.max_t = Ray_max_t;
        }
    }

    on_update(worker);
}

void Camera::set_parameters(json::Value const& parameters) {
    for (auto& n : parameters.GetObject()) {
        if ("frame_duration" == n.name) {
            frame_duration_ = json::read_float(n.value);
        } else if ("frames_per_second" == n.name) {
            float const fps = json::read_float(n.value);
            if (0.f == fps) {
                frame_duration_ = 0.f;
            } else {
                frame_duration_ = 1.f / fps;
            }
        } else if ("motion_blur" == n.name) {
            motion_blur_ = json::read_bool(n.value);
        } else {
            set_parameter(n.name.GetString(), n.value);
        }
    }
}

int2 Camera::resolution() const {
    return resolution_;
}

rendering::sensor::Sensor& Camera::sensor() const {
    return *sensor_;
}

void Camera::set_sensor(std::unique_ptr<rendering::sensor::Sensor> sensor) {
    sensor_ = std::move(sensor);
}

prop::Interface_stack const& Camera::interface_stack() const {
    return interface_stack_;
}

float Camera::frame_duration() const {
    return frame_duration_;
}

void Camera::set_frame_duration(float frame_duration) {
    frame_duration_ = frame_duration;
}

bool Camera::motion_blur() const {
    return motion_blur_;
}

void Camera::set_motion_blur(bool motion_blur) {
    motion_blur_ = motion_blur;
}

void Camera::on_set_transformation() {}

Ray Camera::create_ray(f_float3 origin, f_float3 direction, float time) {
    return Ray(origin, direction, 0.f, Ray_max_t, 0, time, 0.f);
}

}  // namespace scene::camera
