#pragma once

#include "vector.hpp"
#include "matrix.hpp"
#include <ostream>

template<typename T>
std::ostream& operator<<(std::ostream& stream, const math::Vector2<T>& v) {
	return stream << "[" << v.x << ", " << v.y << "]";
}

template<typename T>
std::ostream& operator<<(std::ostream& stream, const math::Vector3<T>& v) {
	return stream << "[" << v.x << ", " << v.y << ", " << v.z << "]";
}

inline std::ostream& operator<<(std::ostream& stream, const math::Vector3f_a& v) {
	return stream << "[" << v.x << ", " << v.y << ", " << v.z << "]";
}

template<typename T>
std::ostream& operator<<(std::ostream& stream, const math::Matrix4x4<T>& m) {
	return stream << "[" << m.m00 << ", " << m.m01 << ", " << m.m02 << ", " << m.m03 << ",\n"
				  << " " << m.m10 << ", " << m.m11 << ", " << m.m12 << ", " << m.m13 << ",\n"
				  << " " << m.m20 << ", " << m.m21 << ", " << m.m22 << ", " << m.m23 << ",\n"
				  << " " << m.m30 << ", " << m.m31 << ", " << m.m32 << ", " << m.m33 << "]";
}
