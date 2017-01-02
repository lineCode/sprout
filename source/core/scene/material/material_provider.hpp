#pragma once

#include "resource/resource_provider.hpp"
#include "material_sample_cache.hpp"
#include "image/texture/texture_types.hpp"
#include "substitute/substitute_coating_sample.hpp"
#include "base/json/json_types.hpp"
#include "base/math/vector.hpp"
#include <vector>

namespace scene { namespace material {

namespace cloth				{ class Sample; }
namespace display			{ class Sample; }
namespace glass				{ class Sample; class Sample_rough; }
namespace light				{ class Sample; }
namespace matte				{ class Sample; }
namespace metal				{ class Sample_isotropic; class Sample_anisotropic; }
namespace metallic_paint	{ class Sample; }

namespace substitute {

class Sample;
class Sample_subsurface;
class Sample_translucent;

}

class Material;

struct Sampler_settings;

class Provider : public resource::Provider<Material> {

public:

	Provider(uint32_t num_threads);
	~Provider();

	virtual std::shared_ptr<Material> load(const std::string& filename,
										   const memory::Variant_map& options,
										   resource::Manager& manager) final override;

	virtual std::shared_ptr<Material> load(const void* data,
										   const std::string& mount_folder,
										   const memory::Variant_map& options,
										   resource::Manager& manager) final override;

	std::shared_ptr<Material> fallback_material() const;

	Sample_cache<light::Sample>& light_cache();

private:

	std::shared_ptr<Material> load(const json::Value& value,
								   const std::string& mount_folder,
								   resource::Manager& manager);


	std::shared_ptr<Material> load_cloth(const json::Value& cloth_value,
										 resource::Manager& manager);

	std::shared_ptr<Material> load_display(const json::Value& display_value,
										   resource::Manager& manager);

	std::shared_ptr<Material> load_glass(const json::Value& glass_value,
										 resource::Manager& manager);

	std::shared_ptr<Material> load_light(const json::Value& light_value,
										 resource::Manager& manager);

	std::shared_ptr<Material> load_matte(const json::Value& metal_value,
										 resource::Manager& manager);

	std::shared_ptr<Material> load_metal(const json::Value& metal_value,
										 resource::Manager& manager);

	std::shared_ptr<Material> load_metallic_paint(const json::Value& metal_value,
												  resource::Manager& manager);

	std::shared_ptr<Material> load_sky(const json::Value& sky_value,
									   resource::Manager& manager);

	std::shared_ptr<Material> load_substitute(const json::Value& substitute_value,
											  resource::Manager& manager);

	struct Texture_description {
		std::string filename;
		std::string usage;
		float2		scale;
		int32_t     num_elements;
	};

	static void read_sampler_settings(const json::Value& sampler_value,
									  Sampler_settings& settings);

	static void read_texture_description(const json::Value& texture_value,
										 Texture_description& description);

	static Texture_adapter create_texture(const Texture_description& description,
										  const memory::Variant_map& options,
										  resource::Manager& manager);

	struct Coating_description {
		float3 color = float3(1.f);
		float  ior = 1.f;
		float  roughness = 0.f;
		float  thickness = 0.f;
		float  weight = 1.f;
		Texture_description normal_map_description;
		Texture_description weight_map_description;
	};

	static void read_coating_description(const json::Value& clearcoat_value,
										 Coating_description& description);

	static float3 read_spectrum(const json::Value& spectrum_value);

	Sample_cache<cloth::Sample>						cloth_cache_;
	Sample_cache<display::Sample>					display_cache_;
	Sample_cache<glass::Sample>						glass_cache_;
	Sample_cache<glass::Sample_rough>				glass_rough_cache_;
	Sample_cache<light::Sample>						light_cache_;
	Sample_cache<matte::Sample>						matte_cache_;
	Sample_cache<metal::Sample_isotropic>			metal_iso_cache_;
	Sample_cache<metal::Sample_anisotropic>			metal_aniso_cache_;
	Sample_cache<metallic_paint::Sample>			metallic_paint_cache_;
	Sample_cache<substitute::Sample>				substitute_cache_;
	Sample_cache<substitute::Sample_clearcoat>		substitute_clearcoat_cache_;
	Sample_cache<substitute::Sample_subsurface>		substitute_subsurface_cache_;
	Sample_cache<substitute::Sample_thinfilm>		substitute_thinfilm_cache_;
	Sample_cache<substitute::Sample_translucent>	substitute_translucent_cache_;

	std::shared_ptr<material::Material> fallback_material_;
};

}}
