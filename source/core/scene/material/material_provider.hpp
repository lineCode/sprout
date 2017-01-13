#pragma once

#include "resource/resource_provider.hpp"
#include "material_sample_cache.hpp"
#include "image/texture/texture_types.hpp"
#include "base/json/json_types.hpp"
#include "base/math/vector.hpp"
#include <vector>

namespace scene { namespace material {

class Material;

using Material_ptr = std::shared_ptr<Material>;

struct Sampler_settings;

class Provider : public resource::Provider<Material> {

public:

	Provider(uint32_t num_threads);
	~Provider();

	virtual Material_ptr load(const std::string& filename, const memory::Variant_map& options,
							  resource::Manager& manager) final override;

	virtual Material_ptr load(const void* data, const std::string& mount_folder,
							  const memory::Variant_map& options,
							  resource::Manager& manager) final override;

	Material_ptr fallback_material() const;

	Sample_cache& sample_cache();

private:

	Material_ptr load(const json::Value& value, const std::string& mount_folder,
					  resource::Manager& manager);

	Material_ptr load_cloth(const json::Value& cloth_value, resource::Manager& manager);

	Material_ptr load_display(const json::Value& display_value, resource::Manager& manager);

	Material_ptr load_glass(const json::Value& glass_value, resource::Manager& manager);

	Material_ptr load_light(const json::Value& light_value, resource::Manager& manager);

	Material_ptr load_matte(const json::Value& metal_value, resource::Manager& manager);

	Material_ptr load_metal(const json::Value& metal_value, resource::Manager& manager);

	Material_ptr load_metallic_paint(const json::Value& paint_value, resource::Manager& manager);

	Material_ptr load_sky(const json::Value& sky_value, resource::Manager& manager);

	Material_ptr load_substitute(const json::Value& substitute_value, resource::Manager& manager);

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

	static uint32_t max_sample_size();

	Sample_cache sample_cache_;

	Material_ptr fallback_material_;
};

}}
