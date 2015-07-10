#pragma once

#include "resource/resource_provider.hpp"
#include "resource/resource_cache.hpp"
#include "image/image_provider.hpp"

namespace image {

class Image;

namespace texture {

class Texture_2D;

class Provider : public resource::Provider<Texture_2D> {
public:

	Provider(file::System& file_system, thread::Pool& pool);

	enum class Flags {
		None = 0,
		Use_as_normal = 1,
		Use_as_mask   = 2
	};

	virtual std::shared_ptr<Texture_2D> load(const std::string& filename, uint32_t flags = 0) final override;

private:

	image::Provider image_provider_;
	resource::Cache<Image> image_cache_;
};

}}


