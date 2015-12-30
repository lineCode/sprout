#pragma once

#include "base/json/rapidjson_types.hpp"
#include "base/math/vector.hpp"
#include <istream>
#include <string>
#include <memory>

namespace exporting { class Sink; }

namespace math { namespace random { class Generator; }}

namespace scene { namespace camera { class Camera; } }

namespace sampler { class Sampler; }

namespace rendering {

struct Focus;

namespace sensor {

class Sensor;

namespace tonemapping { class Tonemapper; }

namespace filter { class Filter; }

}

class Surface_integrator_factory;

}

namespace take {

struct Take;
struct Settings;

class Loader {
public:

	std::shared_ptr<Take> load(std::istream& stream);

private:

	struct Stereoscopic {
		float interpupillary_distance = 0.f;
	};

	void load_camera(const rapidjson::Value& camera_value, bool alpha_transparency, Take& take) const;

	void load_stereoscopic(const rapidjson::Value& stereo_value, Stereoscopic& stereo) const;

	rendering::sensor::Sensor* load_sensor(const rapidjson::Value& sensor_value,
										   math::int2 dimensions, bool alpha_transparency) const;

	std::unique_ptr<rendering::sensor::tonemapping::Tonemapper>
	load_tonemapper(const rapidjson::Value& tonemapper_value) const;

	std::shared_ptr<sampler::Sampler> load_sampler(const rapidjson::Value& sampler_value,
												   math::random::Generator& rng) const;

	std::shared_ptr<rendering::Surface_integrator_factory>
	load_surface_integrator_factory(const rapidjson::Value& integrator_value,
									const Settings& settings) const;

	bool peek_alpha_transparency(const rapidjson::Value& take_value) const;

	std::unique_ptr<exporting::Sink> load_exporter(const rapidjson::Value& exporter_value,
												   scene::camera::Camera& camera) const;

	void load_settings(const rapidjson::Value& settings_value, Settings& settings) const;
};

}
