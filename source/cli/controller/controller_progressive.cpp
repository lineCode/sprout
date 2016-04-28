#include "controller_progressive.hpp"
#include "core/logging/logging.hpp"
#include "core/rendering/rendering_driver_progressive.hpp"
#include "core/scene/scene.hpp"
#include "core/take/take.hpp"
#include "core/progress/progress_sink.hpp"
#include "base/math/vector.inl"
#include "base/string/string.inl"
#include "base/thread/thread_pool.hpp"
#include <iostream>

namespace controller {

void progressive(const take::Take& take, scene::Scene& scene, thread::Pool& thread_pool) {
	logging::info("Progressive mode... type stuff to interact");

	if (!take.view.camera) {
		return;
	}

	rendering::Driver_progressive driver(take.surface_integrator_factory,
										 take.volume_integrator_factory,
										 take.sampler, scene, take.view,
										 thread_pool);

	driver.render(*take.exporter);

	for (;;) {
		std::string input;
		std::cin >> input;

		if ("abort" == input || "exit" == input || "quit" == input) {
			break;
		} else if ("restart" == input) {
			driver.restart();
		} else if ("iteration" == input) {
			logging::info(string::to_string(driver.iteration()));
		} else if ("export" == input) {
			driver.schedule_export();
		}
	}

	driver.abort();
}

}
