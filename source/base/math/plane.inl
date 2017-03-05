#pragma once

#include "plane.hpp"
#include "vector3.inl"
#include "vector4.inl"

namespace math {

/****************************************************************************
 *
 * Generic 3D plane
 *
 ****************************************************************************/

template<typename T>
Plane<T>::Plane() {}

template<typename T>
Plane<T>::Plane(T a, T b, T c, T d) : a(a), b(b), c(c), d(d) {}

template<typename T>
Plane<T>::Plane(const Vector3<T>& normal, T d) : a(normal.x), b(normal.y), c(normal.z), d(d) {}

template<typename T>
Plane<T>::Plane(const Vector3<T>& normal, const Vector3<T>& point) :
	a(normal.x), b(normal.y), c(normal.z), d(-dot(normal, point)) {}

template<typename T>
Plane<T>::Plane(const Vector3<T>& v0, const Vector3<T>& v1, const Vector3<T>& v2) {
	Vector3<T> n = normalized(cross(v2 - v1, v0 - v1));
	a = n.x;
	b = n.y;
	c = n.z;

	d = -dot(n, v0);
}

template<typename T>
T dot(const Plane<T>& p, const Vector3<T>& v) {
	return p.a * v.x + p.b * v.y + p.c * v.z + p.d;
}

template<typename T>
bool behind(const Plane<T>& p, const Vector3<T>& point) {
	return dot(p, point) < 0.f;
}

/****************************************************************************
 *
 * Aligned 3D float plane
 *
 ****************************************************************************/

inline plane create_plane(FVector3f_a normal, float d) {
	return plane(normal, d);
}

inline plane create_plane(FVector3f_a normal, FVector3f_a point) {
	return plane(normal[0], normal[1], normal[2], -dot(normal, point));
}

inline plane create_plane(FVector3f_a v0, FVector3f_a v1, FVector3f_a v2) {
	Vector3f_a n = normalized(cross(v2 - v1, v0 - v1));

	return create_plane(n, v0);
}

inline float dot(FVector4f_a p, FVector3f_a v) {
	return p[0] * v[0] + p[1] * v[1] + p[2] * v[2] + p[3];
}

inline bool behind(FVector4f_a p, FVector3f_a point) {
	return dot(p, point) < 0.f;
}

}
