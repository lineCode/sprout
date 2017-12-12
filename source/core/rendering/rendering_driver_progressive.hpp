#pragma once

#include "rendering_driver.hpp"
#include "exporting/exporting_sink_statistics.hpp"
#include <thread>

namespace scene { namespace camera { class Camera; } }

namespace exporting { class Sink; }

namespace progress { class Sink; }

namespace rendering {

class Driver_progressive : public Driver {

public:

	Driver_progressive(take::Take& take, scene::Scene& scene,
					   thread::Pool& thread_pool, uint32_t max_sample_size);

	void render(exporting::Sink& exporter);

	void abort();

	void schedule_restart(bool recompile);
	void schedule_statistics();

	void set_force_statistics(bool force);

	uint32_t iteration() const;

private:

	bool render_loop(exporting::Sink& exporter);

	void restart();

	std::thread render_thread_;

	uint32_t iteration_;

	uint32_t samples_per_iteration_;

	bool rendering_;

	struct Schedule {
		bool restart	= false;
		bool recompile  = false;
		bool statistics = false;
	};

	Schedule schedule_;

	bool force_statistics_;

	exporting::Statistics statistics_;
};

}
