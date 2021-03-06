#pragma once

#include <cstdio>
#include <string>
#include "exporting_sink.hpp"
#include "image/encoding/encoding_srgb.hpp"

namespace exporting {

class Ffmpeg : public Sink, image::encoding::Srgb {
  public:
    Ffmpeg(std::string const& filename, int2 dimensions, uint32_t framerate);
    ~Ffmpeg();

    virtual void write(const image::Float4& image, uint32_t frame,
                       thread::Pool& pool) override final;

  private:
    FILE* stream_;
};

}  // namespace exporting
