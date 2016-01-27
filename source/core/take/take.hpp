#pragma once

#include "take_settings.hpp"
#include "take_view.hpp"
#include "base/math/random/generator.hpp"
#include <string>
#include <memory>

namespace exporting { class Sink; }

namespace scene { namespace animation { class Animation; } }

namespace rendering { namespace integrator {

namespace surface { class Integrator_factory; }
namespace volume  { class Integrator_factory; }

}}

namespace sampler { class Sampler; }

namespace take {

struct Take {
	Take();

	Settings	settings;
	std::string	scene;
	take::View	view;

	std::shared_ptr<scene::animation::Animation> camera_animation;

	std::shared_ptr<rendering::integrator::surface::Integrator_factory> surface_integrator_factory;
	std::shared_ptr<rendering::integrator::volume::Integrator_factory>  volume_integrator_factory;

	std::shared_ptr<sampler::Sampler> sampler;

	std::unique_ptr<exporting::Sink> exporter;

	math::random::Generator rng;
};

}
