#include "triangle_mesh_exporter.hpp"
#include "triangle_json_handler.hpp"
#include "base/math/vector.inl"
#include <fstream>
#include <sstream>
#include <iostream>

namespace scene { namespace shape { namespace triangle {

std::string extract_filename(const std::string& filename) {
	size_t i = filename.find_last_of('/') + 1;
	return filename.substr(i, filename.find_first_of('.') - i);
}

void newline(std::ostream& stream, uint32_t num_tabs) {
	stream << std::endl;

	for (uint32_t i = 0; i < num_tabs; ++i) {
		stream << '\t';
	}
}

void binary_tag(std::ostream& stream, size_t offset, size_t size) {
	stream << "\"binary\":{\"offset\":" << offset <<",\"size\":" << size << "}";
}

void Exporter::write(const std::string& filename, const Json_handler& handler) {
	std::string out_name = extract_filename(filename) + ".json";

	std::cout << "Export " << out_name << std::endl;

	std::ofstream stream(out_name, std::ios::binary);

	if (!stream) {
		return;
	}

	static char const header[] = "SUM\005";
	stream.write(header, sizeof(char) * 4);

	std::stringstream jstream;

	newline(jstream, 0);
	jstream << "{";

	newline(jstream, 1);
	jstream << "\"geometry\":{";

	newline(jstream, 2);
	jstream << "\"parts\":[";

	const auto& parts = handler.parts();

	for (size_t i = 0, len = parts.size(); i < len; ++i) {
		newline(jstream, 3);

		const auto& p = parts[i];
		jstream << "{";
		jstream << "\"material_index\":" << p.material_index << ",";
		jstream << "\"start_index\":" << p.start_index << ",";
		jstream << "\"num_indices\":" << p.num_indices;
		jstream << "}";

		if (i < len - 1) {
			jstream << ",";
		}
	}


	// close parts
	newline(jstream, 2);
	jstream << "],";

	// vertices
	newline(jstream, 2);
	jstream << "\"vertices\":{";

	newline(jstream, 3);
	const auto& vertices = handler.vertices();
	size_t num_vertices = static_cast<uint32_t>(vertices.size());
	size_t vertices_size = num_vertices * sizeof(Vertex);
	binary_tag(jstream, 0, vertices_size);
	jstream << ",";

	newline(jstream, 3);
	jstream << "\"layout\":[";

	// close layout
	newline(jstream, 3);
	jstream << "]";

	// close vertices
	newline(jstream, 2);
	jstream << "},";

	// indices
	newline(jstream, 2);
	jstream << "\"indices\":{";

	newline(jstream, 3);
	size_t num_indices = handler.triangles().size() * 3;
	binary_tag(jstream, vertices_size, num_indices * sizeof(uint32_t));
	jstream << ",";

	newline(jstream, 3);
	jstream << "\"encoding\":\"uint32\"";

	// close vertices
	newline(jstream, 2);
	jstream << "}";

	// close geometry
	newline(jstream, 1);
	jstream << "}";

	// close start
	newline(jstream, 0);
	jstream << "}";

	newline(jstream, 0);

	std::string json_string = jstream.str();
	uint64_t json_size = json_string.size() - 1;
	stream.write(reinterpret_cast<const char*>(&json_size), sizeof(uint64_t));
	stream.write(reinterpret_cast<const char*>(json_string.data()), json_size * sizeof(char));

	// binary stuff
	stream.write(reinterpret_cast<const char*>(vertices.data()), vertices_size);

	const auto& triangles = handler.triangles();

	for (const auto& t : triangles) {
		stream.write(reinterpret_cast<const char*>(&t.a), sizeof(uint32_t));
		stream.write(reinterpret_cast<const char*>(&t.b), sizeof(uint32_t));
		stream.write(reinterpret_cast<const char*>(&t.c), sizeof(uint32_t));
	}
}

}}}
