#pragma once

#include <cstddef>

namespace resource { class Manager; }

namespace scene { class Scene; }

namespace take { struct Take; }

namespace thread { class Pool; }

namespace controller {

size_t progressive(take::Take& take, scene::Scene& scene,
				   resource::Manager& resource_manager,
				   thread::Pool& thread_pool);

}
