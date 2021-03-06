#ifndef SU_CORE_TAKE_LOADER_HPP
#define SU_CORE_TAKE_LOADER_HPP

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>
#include "base/json/json_types.hpp"
#include "base/math/vector.hpp"

namespace exporting {
class Sink;
}

namespace math::random {
class Generator;
}

namespace resource {
class Manager;
}

namespace scene::camera {
class Camera;
}

namespace sampler {
class Factory;
}

namespace rendering {
struct Focus;

namespace sensor {
class Sensor;

namespace filter {
class Filter;
}
}  // namespace sensor

namespace integrator {
namespace surface {
class Factory;
}

namespace volume {
class Factory;
}

struct Light_sampling;
}  // namespace integrator

namespace postprocessor {
class Postprocessor;
}
}  // namespace rendering

namespace take {

struct Take;
struct View;
struct Settings;
struct Photon_settings;

class Loader {
  public:
    static std::unique_ptr<Take> load(std::istream& stream, resource::Manager& manager);

  private:
    struct Stereoscopic {
        float interpupillary_distance = 0.f;
    };

    using Sensor_filter       = rendering::sensor::filter::Filter;
    using Sensor_ptr          = std::unique_ptr<rendering::sensor::Sensor>;
    using Sampler_factory_ptr = std::shared_ptr<sampler::Factory>;
    using Surface_factory_ptr = std::shared_ptr<rendering::integrator::surface::Factory>;
    using Volume_factory_ptr  = std::shared_ptr<rendering::integrator::volume::Factory>;

    static void load_camera(json::Value const& camera_value, Take& take);

    static Sensor_ptr load_sensor(json::Value const& sensor_value, int2 dimensions);

    static Sensor_filter const* load_filter(rapidjson::Value const& filter_value);

    static Sampler_factory_ptr load_sampler_factory(json::Value const& sampler_value,
                                                    uint32_t           num_workers,
                                                    uint32_t&          num_samples_per_pixel);

    static void load_integrator_factories(json::Value const& integrator_value, uint32_t num_workers,
                                          Take& take);

    static Surface_factory_ptr load_surface_integrator_factory(json::Value const& integrator_value,
                                                               Settings const&    settings,
                                                               uint32_t           num_workers);

    static Volume_factory_ptr load_volume_integrator_factory(json::Value const& integrator_value,
                                                             Settings const&    settings,
                                                             uint32_t           num_workers);

    static void load_photon_settings(json::Value const& value, Photon_settings& settings);

    static void load_postprocessors(json::Value const& pp_value, resource::Manager& manager,
                                    Take& take);

    static std::unique_ptr<rendering::postprocessor::Postprocessor> load_tonemapper(
        json::Value const& tonemapper_value);

    static bool peek_stereoscopic(json::Value const& parameters_value);

    static std::vector<std::unique_ptr<exporting::Sink>> load_exporters(
        json::Value const& exporter_value, const View& view);

    static void load_settings(json::Value const& settings_value, Settings& settings);

    static void load_light_sampling(json::Value const&                     parent_value,
                                    rendering::integrator::Light_sampling& sampling);
};

}  // namespace take

#endif
