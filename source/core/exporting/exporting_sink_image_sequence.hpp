#pragma once

#include "exporting_sink.hpp"
#include "image/encoding/encoding_srgb.hpp"
#include <string>

namespace image { class Writer; }

namespace exporting {

class Image_sequence : public Sink {
public:

	Image_sequence(const std::string& filename, image::Writer* writer);
	~Image_sequence();

	virtual void write(const image::Image_float_4& image, uint32_t frame, thread::Pool& pool) final override;

private:

	std::string filename_;

	image::Writer* writer_;
};

}
