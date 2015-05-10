#include "ray.hpp"
#include "math.hpp"

namespace math {

template<typename T>
inline Ray<T>::Ray() {}

template<typename T>
inline Ray<T>::Ray(const Vector3<T>& origin, const Vector3<T>& direction, T min_t, T max_t) :
	origin(origin), direction(direction), min_t(min_t), max_t(max_t)
{}

template<typename T>
inline Vector3<T> Ray<T>::point(T t) const {
	return origin + t * direction;
}

template<typename T>
inline void Optimized_ray<T>::set_direction(const Vector3<T>& v) {
	this->direction = v;
	reciprocal_direction = Vector3<T>(T(1) / v.x, T(1) / v.y, T(1) / v.z);

	sign[0] = static_cast<uint8_t>(std::signbit(v.x));
	sign[1] = static_cast<uint8_t>(std::signbit(v.y));
	sign[2] = static_cast<uint8_t>(std::signbit(v.z));
}

}
