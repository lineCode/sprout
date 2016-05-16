#include "exporting_sink_statistics.hpp"
#include "image/image_helper.hpp"
#include "logging/logging.hpp"
#include "base/math/vector.inl"
#include "base/string/string.inl"

namespace exporting {

Statistics::Statistics() {}

Statistics::~Statistics() {}

void Statistics::write(const image::Image_float_4& image, uint32_t frame, thread::Pool& /*pool*/) {
	logging::info("Frame: " + string::to_string(frame));

	math::float3 average_rgb = image::average_3(image);

	float max_luminance;
	float average_luminance = image::average_and_max_luminance(image, max_luminance);

	logging::info("Average RGB:       " + string::to_string(average_rgb));
	logging::info("Average luminance: " + string::to_string(average_luminance));
	logging::info("Max     luminance: " + string::to_string(max_luminance));
}

}