#pragma once

#include <string>
#include <iosfwd>

namespace scene { namespace shape { namespace triangle {

class Vertex_layout_description {

public:

	enum class Encoding {
		Float32x1,
		Float32x2,
		Float32x3
	};

	struct Element {
		std::string semantic_name;
		uint32_t semantic_index = 0;
		Encoding encoding;
		uint32_t stream = 0;
		uint32_t byte_offset = 0;
	};
};

}}}

/*
std::stringstream& operator<<(std::stringstream& stream,
							  scene::shape::triangle::Vertex_layout_description::Encoding encoding);
							  */

std::stringstream& operator<<(std::stringstream& stream,
							  const scene::shape::triangle::Vertex_layout_description::Element& element);