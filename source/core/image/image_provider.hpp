#ifndef SU_CORE_IMAGE_PROVIDER_HPP
#define SU_CORE_IMAGE_PROVIDER_HPP

#include "encoding/png/png_reader.hpp"
#include "procedural/flakes/flakes_provider.hpp"
#include "resource/resource_provider.hpp"

namespace image {

class Image;

class Provider final : public resource::Provider<Image> {
  public:
    Provider() noexcept;

    ~Provider() noexcept override final;

    std::shared_ptr<Image> load(std::string const& filename, memory::Variant_map const& options,
                                resource::Manager& manager) override final;

    std::shared_ptr<Image> load(void const* data, std::string_view mount_folder,
                                memory::Variant_map const& options,
                                resource::Manager&         manager) override final;

    size_t num_bytes() const noexcept override final;

  private:
    encoding::png::Reader png_reader_;

    procedural::flakes::Provider flakes_provider_;
};

}  // namespace image

#endif
