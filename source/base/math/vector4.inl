#pragma once

#include "vector4.hpp"

namespace math {

/****************************************************************************
 *
 * Generic 4D vector
 *
 ****************************************************************************/

template<typename T>
Vector4<T>::Vector4() {}

template<typename T>
Vector4<T>::Vector4(T x, T y, T z, T w) : v{x, y, z, w} {}

template<typename T>
Vector4<T>::Vector4(T s) : v{s, s, s, s} {}

template<typename T>
Vector4<T>::Vector4(Vector2<T> xy, T z, T w) : v{xy[0], xy[1], z, w} {}

template<typename T>
Vector4<T>::Vector4(const Vector3<T>& xyz, T w) : v{xyz[0], xyz[1], xyz[2], w} {}

template<typename T>
T Vector4<T>::operator[](uint32_t i) const{
	return v[i];
}

template<typename T>
T& Vector4<T>::operator[](uint32_t i) {
	return v[i];
}

template<typename T>
Vector4<T> Vector4<T>::operator+(const Vector4& v) const {
	return Vector4(v[0] + v[0], v[1] + v[1], v[2] + v[2], v[3] + v[3]);
}

template<typename T>
Vector4<T> Vector4<T>::operator*(const Vector4& v) const {
	return Vector4(v[0] * v[0], v[1] * v[1], v[2] * v[2], v[3] * v[3]);
}

template<typename T>
Vector4<T> Vector4<T>::operator/(T s) const {
	T is = T(1) / s;
	return Vector4(is * v[0], is * v[1], is * v[2], is * v[2]);
}

template<typename T>
Vector4<T>& Vector4<T>::operator+=(const Vector4& v) {
	v[0] += v[0]; v[1] += v[1]; v[2] += v[2]; v[3] += v[3];
	return *this;
}

template<typename T>
Vector4<T>& Vector4<T>::operator-=(const Vector4& v) {
	v[0] -= v[0]; v[1] -= v[1]; v[2] -= v[2]; v[3] -= v[3];
	return *this;
}

template<typename T>
const Vector4<T> Vector4<T>::identity(T(0), T(0), T(0), T(0));

template<typename T>
Vector4<T> operator*(T s, const Vector4<T> &v) {
	return Vector4<T>(s * v[0], s * v[1], s * v[2], s * v[3]);
}

template<typename T>
T dot(const Vector4<T>& a, const Vector4<T>& b) {
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
}

/****************************************************************************
 *
 * Aligned 4D float vector
 *
 ****************************************************************************/

inline Vector4f_a::Vector4f_a() {}

inline Vector4f_a::Vector4f_a(float x, float y, float z, float w) :
	v{x, y, z, w} {}

inline Vector4f_a::Vector4f_a(float s) : v{s, s, s, s} {}

inline Vector4f_a::Vector4f_a(Vector2<float> xy, float z, float w) :
	v{xy[0], xy[1], z, w} {}

inline Vector4f_a::Vector4f_a(FVector3f_a xyz, float w) :
	v{xyz[0], xyz[1], xyz[2], w} {}

inline Vector4f_a::Vector4f_a(const Vector3<float>& xyz, float w) :
	v{xyz.x, xyz.y, xyz.z, w} {}

inline Vector3f_a Vector4f_a::xyz() const {
	return Vector3f_a(v);
}

inline float Vector4f_a::operator[](uint32_t i) const{
	return v[i];
}

inline float& Vector4f_a::operator[](uint32_t i) {
	return v[i];
}

inline Vector4f_a Vector4f_a::operator+(const Vector4f_a& a) const {
	return Vector4f_a(v[0] + a[0], v[1] + a[1], v[2] + a[2], v[3] + a[3]);
}

inline Vector4f_a Vector4f_a::operator*(const Vector4f_a& a) const {
	return Vector4f_a(v[0] * a[0], v[1] * a[1], v[2] * a[2], v[3] * a[3]);
}

inline Vector4f_a Vector4f_a::operator/(float s) const {
	float is = 1.f / s;
	return Vector4f_a(is * v[0], is * v[1], is * v[2], is * v[3]);
}

inline Vector4f_a& Vector4f_a::operator+=(const Vector4f_a& a) {
	v[0] += a[0]; v[1] += a[1]; v[2] += a[2]; v[3] += a[3];
	return *this;
}

inline Vector4f_a& Vector4f_a::operator-=(const Vector4f_a& a) {
	v[0] -= a[0]; v[1] -= a[1]; v[2] -= a[2]; v[3] -= a[3];
	return *this;
}

inline bool Vector4f_a::operator==(FVector4f_a a) const {
	return v[0] == a[0] && v[1] == a[1] && v[2] == a[2] && v[3] == a[3];
}

inline bool Vector4f_a::operator!=(FVector4f_a a) const {
	return v[0] != a[0] || v[1] != a[1] || v[2] != a[2] || v[3] != a[3];
}

inline Vector4f_a operator*(float s, const Vector4f_a& a) {
	return Vector4f_a(s * a[0], s * a[1], s * a[2], s * a[3]);
}

inline float dot(const Vector4f_a& a, const Vector4f_a& b) {
	return (a[0] * b[0] + a[1] * b[1]) + (a[2] * b[2] + a[3] * b[3]);
}

/****************************************************************************
 *
 * Aligned 4D float vector
 *
 ****************************************************************************/

inline Vector4i_a::Vector4i_a() {}

inline Vector4i_a::Vector4i_a(int32_t x, int32_t y, int32_t z, int32_t w) :
	v{x, y, z, w} {}

inline Vector4i_a::Vector4i_a(Vector2<int32_t> xy, Vector2<int32_t> zw) :
	v{xy[0], xy[1], zw[0], zw[1]} {}

inline Vector4i_a::Vector4i_a(int32_t s) : v{s, s, s, s} {}

inline const Vector2<int32_t> Vector4i_a::xy() const {
	return Vector2<int32_t>(v[0], v[1]);
}

inline const Vector2<int32_t> Vector4i_a::zw() const {
	return Vector2<int32_t>(v[2], v[3]);
}

inline int32_t Vector4i_a::operator[](uint32_t i) const{
	return v[i];
}

inline int32_t& Vector4i_a::operator[](uint32_t i) {
	return v[i];
}

inline Vector4i_a Vector4i_a::operator+(const Vector4i_a& a) const {
	return Vector4i_a(v[0] + a[0], v[1] + a[1], v[2] + a[2], v[3] + a[3]);
}

inline Vector4i_a Vector4i_a::operator*(const Vector4i_a& a) const {
	return Vector4i_a(v[0] * a[0], v[1] * a[1], v[2] * a[2], v[3] * a[3]);
}

inline Vector4i_a& Vector4i_a::operator+=(const Vector4i_a& a) {
	v[0] += a[0]; v[1] += a[1]; v[2] += a[2]; v[3] += a[3];
	return *this;
}

inline Vector4i_a& Vector4i_a::operator-=(const Vector4i_a& a) {
	v[0] -= a[0]; v[1] -= a[1]; v[2] -= a[2]; v[3] -= a[3];
	return *this;
}

}
