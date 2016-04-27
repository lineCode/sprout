#include "rendering_driver_progressive.hpp"
#include "exporting/exporting_sink.hpp"
#include "image/typed_image.inl"
#include "logging/logging.hpp"
#include "progress/progress_sink.hpp"
#include "rendering/sensor/sensor.hpp"
#include "rendering/integrator/integrator.hpp"
#include "sampler/sampler.hpp"
#include "scene/scene.hpp"
#include "scene/camera/camera.hpp"
#include "take/take_view.hpp"
#include "base/chrono/chrono.hpp"
#include "base/math/vector.inl"
#include "base/math/random/generator.inl"
#include "base/string/string.inl"
#include "base/thread/thread_pool.hpp"

namespace rendering {

Driver_progressive::Driver_progressive(Surface_integrator_factory surface_integrator_factory,
									   Volume_integrator_factory volume_integrator_factory,
									   std::shared_ptr<sampler::Sampler> sampler,
									   scene::Scene& scene, const take::View& view,
									   thread::Pool& thread_pool) :
	Driver(surface_integrator_factory, volume_integrator_factory,
		   sampler, scene, view, thread_pool) {}

void Driver_progressive::render(exporting::Sink& exporter, progress::Sink& progressor) {
	auto& camera = *view_.camera;
	auto& sensor = camera.sensor();

	scene_.tick(thread_pool_);

	camera.update_focus(workers_[0]);

	sensor.clear();

	rendering_ = true;

	render_thread_ = std::thread([this, &exporter, &progressor](){
		for (;rendering_;) {
			render_loop(exporter, progressor);
		}
	});
}

void Driver_progressive::abort() {
	rendering_ = false;

	render_thread_.join();
}

void Driver_progressive::render_loop(exporting::Sink& exporter, progress::Sink& progressor) {
	auto& camera = *view_.camera;
	auto& sensor = camera.sensor();

	for (uint32_t v = 0, len = view_.camera->num_views(); v < len; ++v) {
		tiles_.restart();

		thread_pool_.run([this, v](uint32_t index) {
				auto& worker = workers_[index];

				for (;;) {
					math::Recti tile;
					if (!tiles_.pop(tile)) {
						break;
					}

					worker.render(*view_.camera, v, tile, 0, 1, 0.f, 1.f);
				}
			}
		);
	}

	sensor.resolve(thread_pool_, target_);
	exporter.write(target_, 0, thread_pool_);

	progressor.tick();
}

}