#include "sky_provider.hpp"
#include "sky.hpp"
#include "sky_material.hpp"
#include "core/scene/scene.hpp"
#include "core/scene/scene_loader.hpp"
#include "core/scene/material/material_provider.hpp"
#include "core/scene/material/light/light_constant.hpp"
#include "core/scene/prop/prop.hpp"
#include "base/json/json.hpp"
#include "base/math/vector.inl"
#include "base/math/quaternion.inl"
#include <iostream>

namespace procedural { namespace sky {

Provider provider;

void init(scene::Loader& loader, scene::material::Provider& material_provider) {
	provider.set_scene_loader(loader);
	provider.set_material_provider(material_provider);

	loader.register_extension_provider("Sky", &provider);
}

Provider::Provider() : material_provider_(nullptr) {}

void Provider::set_scene_loader(scene::Loader& loader) {
	scene_loader_ = &loader;
}

void Provider::set_material_provider(scene::material::Provider& provider) {
	material_provider_ = &provider;
}

scene::entity::Entity* Provider::create_extension(const json::Value& extension_value,
												  scene::Scene& scene,
												  resource::Manager& /*manager*/) {
	scene::Prop* sky_prop = scene.create_prop(scene_loader_->canopy());
	scene::Prop* sun_prop = scene.create_prop(scene_loader_->celestial_disk());

	Sky* sky = new Sky(*sky_prop, *sun_prop);

	auto sky_material = std::make_shared<Material>(material_provider_->light_cache(), sky->model());

	auto sun_material = material_provider_->create_light();
	sun_material->emittance().set_radiance(math::float3(2477907.f, 2133774.f, 1763460.f));

	const json::Value::ConstMemberIterator p = extension_value.FindMember("parameters");
	if (extension_value.MemberEnd() != p) {
		sky->set_parameters(p->value);
	}

	bool visible_in_camera = true;
	bool visible_in_reflection = true;
	bool visible_in_shadow = true;

	scene::material::Materials materials(1);

	materials[0] = sky_material;
	sky_prop->set_materials(materials);
	sky_prop->set_visibility(visible_in_camera, visible_in_reflection, visible_in_shadow);
	sky_prop->set_open(false);
	scene.create_prop_light(sky_prop, 0);

	materials[0] = sun_material;
	sun_prop->set_materials(materials);
	sun_prop->set_visibility(visible_in_camera, visible_in_reflection, visible_in_shadow);
	sun_prop->set_open(false);
	scene.create_prop_light(sun_prop, 0);

	return sky;
}

}}