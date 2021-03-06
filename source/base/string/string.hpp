#ifndef SU_BASE_STRING_STRING_HPP
#define SU_BASE_STRING_STRING_HPP

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>
#include "math/print.hpp"

namespace string {

template <typename T>
static std::string to_string(T value, int num_padded_zeros = 0) {
    std::ostringstream stream;

    if (num_padded_zeros) {
        stream << std::setw(num_padded_zeros) << std::setfill('0');
    }

    stream << value;
    return stream.str();
}

static inline bool is_space(char c) {
    return std::isspace(static_cast<int>(c)) != 0;
}

static inline void trim(std::string& text) {
    auto const begin = std::find_if_not(text.begin(), text.end(), is_space);
    auto const end   = std::find_if_not(text.rbegin(), text.rend(), is_space).base();

    text.assign(begin, end);
}

static inline std::string parent_directory(std::string const& filename) {
    size_t const i = filename.find_last_of('/');
    return filename.substr(0, i + 1);
}

static inline std::string_view suffix(std::string_view filename) {
    size_t const i = filename.find_last_of('.');
    return filename.substr(i + 1, std::string::npos);
}

static inline std::string_view presuffix(std::string_view filename) {
    size_t const i = filename.find_last_of('.');
    size_t const j = filename.substr(0, i).find_last_of('.') + 1;
    return filename.substr(j, i - j);
}

static inline std::string print_bytes(size_t num_bytes) {
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

}  // namespace string

#endif
