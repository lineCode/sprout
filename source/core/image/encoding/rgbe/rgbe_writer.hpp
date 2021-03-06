#pragma once

#include "image/image_writer.hpp"

namespace image::encoding::rgbe {

class Writer : public image::Writer {
  public:
    //	Writer(math::uint2 dimensions);

    virtual std::string file_extension() const override final;

    virtual bool write(std::ostream& stream, Float4 const& image,
                       thread::Pool& pool) override final;

  private:
    static void write_header(std::ostream& stream, int2 dimensions);

    static void write_pixels(std::ostream& stream, Float4 const& image);

    static void write_pixels_rle(std::ostream& stream, Float4 const& image);

    static void write_bytes_rle(std::ostream& stream, uint8_t const* data, uint32_t num_bytes);

    static byte4 float_to_rgbe(float4 const& c);
};

}  // namespace image::encoding::rgbe
