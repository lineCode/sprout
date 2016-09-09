#include "string.hpp"
#include "math/print.hpp"
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>

namespace string {

template<typename T>
std::string to_string(T value, int num_padded_zeros) {
	std::ostringstream stream;

	if (num_padded_zeros) {
		stream << std::setw(num_padded_zeros) << std::setfill('0');
	}

	stream << value;
	return stream.str();
}

inline void trim(std::string& text) {
	auto begin = std::find_if_not(text.begin(),  text.end(),  is_space);
	auto end   = std::find_if_not(text.rbegin(), text.rend(), is_space).base();

	text.assign(begin, end);
}

inline bool is_space(char c) {
	return std::isspace(static_cast<int>(c)) != 0;
}

inline std::string parent_directory(const std::string& filename) {
	size_t i = filename.find_last_of('/');
	return filename.substr(0, i + 1);
}

inline std::string print_bytes(size_t num_bytes) {
	std::ostringstream stream;

	stream << std::fixed << std::setprecision(2);

	if (num_bytes < 1024) {
		stream << num_bytes;
		stream << " B";
	} else if (num_bytes < 1024 * 1024) {
		stream << static_cast<float>(num_bytes) / 1024.f;
		stream << " KiB";
	} else if (num_bytes < 1024 * 1024 * 1024) {
		stream << static_cast<float>(num_bytes) / (1024.f * 1024.f);
		stream << " MiB";
	} else {
		stream << static_cast<float>(num_bytes) / (1024.f * 1024.f * 1024.f);
		stream << " GiB";
	}

	return stream.str();
}

}
