#ifndef SU_CORE_TAKE_TAKE_HPP
#define SU_CORE_TAKE_TAKE_HPP

#include <memory>
#include <string>
#include <vector>
#include "exporting/exporting_sink.hpp"
#include "take_settings.hpp"
#include "take_view.hpp"

namespace scene::animation {
class Animation;
}

namespace rendering::integrator {

namespace surface {
class Factory;
}
namespace volume {
class Factory;
}

}  // namespace rendering::integrator

namespace sampler {
class Factory;
}

namespace take {

struct Take {
    Take() = default;

    Settings    settings;
    std::string scene_filename;
    take::View  view;

    Photon_settings photon_settings;

    std::shared_ptr<scene::animation::Animation> camera_animation;

    std::shared_ptr<rendering::integrator::surface::Factory> surface_integrator_factory;
    std::shared_ptr<rendering::integrator::volume::Factory>  volume_integrator_factory;

    std::shared_ptr<sampler::Factory> sampler_factory;

    std::vector<std::unique_ptr<exporting::Sink>> exporters;
};

}  // namespace take

#endif
