#ifndef SU_EXTENSION_PROCEDURAL_SKY_PROVIDER_HPP
#define SU_EXTENSION_PROCEDURAL_SKY_PROVIDER_HPP

#include "core/scene/entity/entity_extension_provider.hpp"
#include <memory>

namespace scene {

class Loader;

namespace material { class Provider; }

}

namespace procedural::sky {

void init(scene::Loader& loader, scene::material::Provider& material_provider);

class Provider : public scene::entity::Extension_provider {

public:

	Provider();

	void set_scene_loader(scene::Loader& loader);
	void set_material_provider(scene::material::Provider& provider);

	virtual scene::entity::Entity* create_extension(const json::Value& extension_value,
													scene::Scene& scene,
													resource::Manager& manager) override final;

private:

	scene::Loader* scene_loader_;
	scene::material::Provider* material_provider_;
};

}

#endif
