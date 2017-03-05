#include "testing_simd.hpp"
#include "base/chrono/chrono.hpp"
#include "base/math/vector.inl"
#include "base/random/generator.inl"
#include "base/math/simd/simd_math.inl"
#include "base/math/simd/simd_vector.inl"
#include "base/string/string.inl"
#include "base/math/print.hpp"
#include "base/math/simd/simd_math.inl"
#include <iostream>

namespace testing { namespace simd {

inline float SSE_sqrt(float x) {
	_mm_store_ss(&x, _mm_sqrt_ss(_mm_load_ss(&x)));
	return x;
}

inline float SSE_rsqrt(float x) {
	_mm_store_ss(&x, _mm_rsqrt_ss(_mm_load_ss(&x)));
	return x;
}

inline float SSE_rsqrt_1N(float x) {
	float y = SSE_rsqrt(x);
	return y * (1.5f - 0.5f * x * y * y);
}

void rsqrt() {
	std::cout << "testing::simd::rsqrt()" << std::endl;

	rnd::Generator rng(456, 90, 2123, 4598743);

	size_t num_values = 1024 * 1024 * (128 + 32);

	float* values = new float[num_values];

	for (size_t i = 0; i < num_values; ++i) {
		values[i] = 10.f * rng.random_float();
	}

	{
		float a = 0.f;
		auto start = std::chrono::high_resolution_clock::now();

		for (size_t i = 0; i < num_values; ++i) {
			float x = values[i];
			a += (1.f / std::sqrt(x));
			a = (1.f / std::sqrt(a));
		}

		const auto duration = chrono::seconds_since(start);
		std::cout << a << " in " << string::to_string(duration) << " s" << std::endl;
	}

	{
		float a = 0.f;
		auto start = std::chrono::high_resolution_clock::now();

		for (size_t i = 0; i < num_values; ++i) {
			float x = values[i];
			a += math::simd::rsqrt(x);
			a = math::simd::rsqrt(a);
		}

		const auto duration = chrono::seconds_since(start);
		std::cout << a << " in " << string::to_string(duration) << " s" << std::endl;
	}

	{
		float a = 0.f;
		auto start = std::chrono::high_resolution_clock::now();

		for (size_t i = 0; i < num_values; ++i) {
			float x = values[i];
			a += SSE_rsqrt(x);
			a = SSE_rsqrt(a);
		}

		const auto duration = chrono::seconds_since(start);
		std::cout << a << " in " << string::to_string(duration) << " s" << std::endl;
	}

	{
		float a = 0.f;
		auto start = std::chrono::high_resolution_clock::now();

		for (size_t i = 0; i < num_values; ++i) {
			float x = values[i];
			a += (1.f / std::sqrt(x));
			a = (1.f / std::sqrt(a));
		}

		const auto duration = chrono::seconds_since(start);
		std::cout << a << " in " << string::to_string(duration) << " s" << std::endl;
	}

	delete[] values;
}

inline float SSE_rcp(float x) {
	_mm_store_ss(&x, _mm_rcp_ss(_mm_load_ss(&x)));
	return x;
}

inline float SSE_rcp_1N(float x) {
	__m128 sx = _mm_load_ss(&x);
	__m128 res = _mm_rcp_ss(sx);
	__m128 muls = _mm_mul_ss(sx, _mm_mul_ss(res, res));
	_mm_store_ss(&x, _mm_sub_ss(_mm_add_ss(res, res), muls));
	return x;
}

void rcp() {
	std::cout << "testing::simd::rcp()" << std::endl;

	rnd::Generator rng(456, 90, 2123, 4598743);

	size_t num_values = 1024 * 1024 * (128 + 8);

	float* values = new float[num_values];

	for (size_t i = 0; i < num_values; ++i) {
		values[i] = 10.f * rng.random_float();
	}

	{
		float a = 0.f;
		auto start = std::chrono::high_resolution_clock::now();

		for (size_t i = 0; i < num_values; ++i) {
			float x = values[i];
			a += (1.f / x);
			a = (1.f / a);
		}

		const auto duration = chrono::seconds_since(start);
		std::cout << a << " in " << string::to_string(duration) << " s" << std::endl;
	}

	{
		float a = 0.f;
		auto start = std::chrono::high_resolution_clock::now();

		for (size_t i = 0; i < num_values; ++i) {
			float x = values[i];
			a += SSE_rcp(x);
			a = SSE_rcp(a);
		}

		const auto duration = chrono::seconds_since(start);
		std::cout << a << " in " << string::to_string(duration) << " s" << std::endl;
	}

	{
		float a = 0.f;
		auto start = std::chrono::high_resolution_clock::now();

		for (size_t i = 0; i < num_values; ++i) {
			float x = values[i];
			a += SSE_rcp_1N(x);
			a = SSE_rcp_1N(a);
		}

		const auto duration = chrono::seconds_since(start);
		std::cout << a << " in " << string::to_string(duration) << " s" << std::endl;
	}

	{
		float a = 0.f;
		auto start = std::chrono::high_resolution_clock::now();

		for (size_t i = 0; i < num_values; ++i) {
			float x = values[i];
			a += (1.f / x);
			a = (1.f / a);
		}

		const auto duration = chrono::seconds_since(start);
		std::cout << a << " in " << string::to_string(duration) << " s" << std::endl;
	}

	delete[] values;
}

inline float3 simd_normalized_0(const float3& v) {
	float il = math::simd::rsqrt(math::dot(v, v));
	return il * v;
}

inline float3 simd_normalized_1(const float3& v) {
	math::simd::Vector sx = math::simd::load_float3(v);

	math::simd::Vector d = math::simd::dot3(sx, sx);

	math::simd::Vector il = math::simd::rsqrt(d);

	float3 result;
	math::simd::store_float3(result, _mm_mul_ps(il, sx));

	return result;
}

inline float simd_dotlly(const float3& a, const float3& b) {
//	math::simd::Vector sa = math::simd::load_float4(a);
//	math::simd::Vector sb = math::simd::load_float4(b);

//	math::simd::Vector d = _mm_dp_ps(sa, sb, 0x77);

//	float x;
//	_mm_store_ss(&x, d);
//	return x;
	return math::dot(a, b);
}

inline float3 simd_normalized_2(const float3& v) {
//	math::simd::Vector sx = math::simd::load_float3(v);

//	math::simd::Vector d = _mm_dp_ps(sx, sx, 0x77);

//	math::simd::Vector il = math::simd::rsqrt(d);

//	float3 result;
//	math::simd::store_float3(result, _mm_mul_ps(il, sx));

//	return result;

	return math::normalized(v);
}

void normalize() {
	std::cout << "testing::simd::normalize()" << std::endl;

	rnd::Generator rng(456, 90, 2123, 4598743);

	size_t num_values = 1024 * 1024 * (128 + 0);

	float3* vectors = new float3[num_values];

	for (size_t i = 0; i < num_values; ++i) {
		vectors[i] = float3(5.f - (10.f * rng.random_float()),
							5.f - (10.f * rng.random_float()),
							5.f - (10.f * rng.random_float()));
	}

	{
		std::cout << "math::normalized()" << std::endl;
		std::cout << vectors[0] << " : " << math::normalized(vectors[0]) << std::endl;

		auto start = std::chrono::high_resolution_clock::now();

		float3 result(0.f);

		for (size_t i = 0; i < num_values; ++i) {
			float3 x = vectors[i];
			float3 r = math::normalized(x);
			result = math::normalized(r + result);
		}

		const auto duration = chrono::seconds_since(start);
		std::cout << result << " in " << string::to_string(duration) << " s" << std::endl;
		std::cout << std::endl;
	}

	{
		std::cout << "simd_normalized_0()" << std::endl;
		std::cout << vectors[0] << " : " << simd_normalized_0(vectors[0]) << std::endl;

		auto start = std::chrono::high_resolution_clock::now();

		float3 result(0.f);

		for (size_t i = 0; i < num_values; ++i) {
			float3 x = vectors[i];
			float3 r = simd_normalized_0(x);
			result = simd_normalized_0(r + result);
		}

		const auto duration = chrono::seconds_since(start);
		std::cout << result << " in " << string::to_string(duration) << " s" << std::endl;
		std::cout << std::endl;
	}

	{
		std::cout << "simd_normalized_1()" << std::endl;
		std::cout << vectors[0] << " : " << simd_normalized_1(vectors[0]) << std::endl;

		auto start = std::chrono::high_resolution_clock::now();

		float3 result(0.f);

		for (size_t i = 0; i < num_values; ++i) {
			float3 x = vectors[i];
			float3 r = simd_normalized_1(x);
			result = simd_normalized_1(r + result);
		}

		const auto duration = chrono::seconds_since(start);
		std::cout << result << " in " << string::to_string(duration) << " s" << std::endl;
		std::cout << std::endl;
	}

	{
		std::cout << "simd_normalized_2()" << std::endl;
		std::cout << vectors[0] << " : " << simd_normalized_2(vectors[0]) << std::endl;

		auto start = std::chrono::high_resolution_clock::now();

		float3 result(0.f);

		for (size_t i = 0; i < num_values; ++i) {
			float3 x = vectors[i];
			float3 r = simd_normalized_2(x);
			result = simd_normalized_2(r + result);
		}

		const auto duration = chrono::seconds_since(start);
		std::cout << result << " in " << string::to_string(duration) << " s" << std::endl;
		std::cout << std::endl;
	}

	{
		std::cout << "math::normalized()" << std::endl;
		std::cout << vectors[0] << " : " << math::normalized(vectors[0]) << std::endl;

		auto start = std::chrono::high_resolution_clock::now();

		float3 result(0.f);

		for (size_t i = 0; i < num_values; ++i) {
			float3 x = vectors[i];
			float3 r = math::normalized(x);
			result = math::normalized(r + result);
		}

		const auto duration = chrono::seconds_since(start);
		std::cout << result << " in " << string::to_string(duration) << " s" << std::endl;
		std::cout << std::endl;
	}

	delete[] vectors;
}


//inline float SSE_rcp_1N(float x) {
//	__m128 sx = _mm_load_ss(&x);
//	__m128 res = _mm_rcp_ss(sx);
//	__m128 muls = _mm_mul_ss(sx, _mm_mul_ss(res, res));
//	_mm_store_ss(&x, _mm_sub_ss(_mm_add_ss(res, res), muls));
//	return x;
//}

inline float3 simd_reciprocal(const float3& v) {
	math::simd::Vector sx = math::simd::load_float3(v);

	__m128 rcp = _mm_rcp_ps(sx);
	__m128 mul = _mm_mul_ps(sx, _mm_mul_ps(rcp, rcp));

	float3 result;
	math::simd::store_float3(result, _mm_sub_ps(_mm_add_ps(rcp, rcp), mul));
	return result;
}

inline void simd_reciprocal(float3& result, const float3& v) {
	math::simd::Vector sx = math::simd::load_float3(v);

	__m128 rcp = _mm_rcp_ps(sx);
	__m128 mul = _mm_mul_ps(sx, _mm_mul_ps(rcp, rcp));

	math::simd::store_float3(result, _mm_sub_ps(_mm_add_ps(rcp, rcp), mul));
}

void reciprocal() {
	std::cout << "testing::simd::reciprocal()" << std::endl;

	rnd::Generator rng(456, 90, 2123, 4598743);

	size_t num_values = 1024 * 1024 * (9);

	float3* vectors = new float3[num_values];

	for (size_t i = 0; i < num_values; ++i) {
		vectors[i] = float3(5.f - (10.f * rng.random_float()),
							5.f - (10.f * rng.random_float()),
							5.f - (10.f * rng.random_float()));
	}

	{
		std::cout << vectors[0] << " : " << math::reciprocal(vectors[0]) << std::endl;

		auto start = std::chrono::high_resolution_clock::now();

		float3 result(0.f);

		for (size_t i = 0; i < num_values; ++i) {
			float3 x = vectors[i];
			float3 r = math::reciprocal(x);
			result = math::reciprocal(r + result);
		}

		const auto duration = chrono::seconds_since(start);
		std::cout << result << " in " << string::to_string(duration) << " s" << std::endl;
	}

	{
		std::cout << vectors[0] << " : " << simd_reciprocal(vectors[0]) << std::endl;

		auto start = std::chrono::high_resolution_clock::now();

		float3 result(0.f);

		for (size_t i = 0; i < num_values; ++i) {
			float3 x = vectors[i];
			float3 r = simd_reciprocal(x);
			result = simd_reciprocal(r + result);
		}

		const auto duration = chrono::seconds_since(start);
		std::cout << result << " in " << string::to_string(duration) << " s" << std::endl;
	}

	{
		std::cout << vectors[0] << " : " << simd_normalized_1(vectors[0]) << std::endl;

		auto start = std::chrono::high_resolution_clock::now();

		float3 result(0.f);

		for (size_t i = 0; i < num_values; ++i) {
			float3 x = vectors[i];
			float3 r;
			simd_reciprocal(r, x);
			simd_reciprocal(result, r + result);
		}

		const auto duration = chrono::seconds_since(start);
		std::cout << result << " in " << string::to_string(duration) << " s" << std::endl;
	}

	{
		std::cout << vectors[0] << " : " << math::normalized(vectors[0]) << std::endl;

		auto start = std::chrono::high_resolution_clock::now();

		float3 result(0.f);

		for (size_t i = 0; i < num_values; ++i) {
			float3 x = vectors[i];
			float3 r = math::reciprocal(x);
			result = math::reciprocal(r + result);
		}

		const auto duration = chrono::seconds_since(start);
		std::cout << result << " in " << string::to_string(duration) << " s" << std::endl;
	}

	delete[] vectors;
}

inline float dotly(float3 a, float3 b) {
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

inline float simd_dot_0(const float3& a, const float3& b) {
	math::simd::Vector sa = math::simd::load_float3(a);
	math::simd::Vector sb = math::simd::load_float3(b);

	math::simd::Vector d = math::simd::dot3(sa, sb);

	float x;
	_mm_store_ss(&x, d);
	return x;
}

inline float simd_dot_1(const float3 a, const float3 b) {
	math::simd::Vector sa = math::simd::load_float3(a);
	math::simd::Vector sb = math::simd::load_float3(b);

	// Perform the dot product
	math::simd::Vector vDot = _mm_mul_ps(sa, sb);
	// x=Dot.vector4_f32[1], y=Dot.vector4_f32[2]
	math::simd::Vector vTemp = SU_PERMUTE_PS(vDot, _MM_SHUFFLE(2, 1, 2, 1));
	// Result.vector4_f32[0] = x+y
	vDot = _mm_add_ss(vDot, vTemp);
	// x=Dot.vector4_f32[2]
	vTemp = SU_PERMUTE_PS(vTemp, _MM_SHUFFLE(1, 1, 1, 1));
	// Result.vector4_f32[0] = (x+y)+z
	vDot = _mm_add_ss(vDot, vTemp);
	// Splat x

	float x;
	_mm_store_ss(&x, vDot);
	return x;

//	return SU_PERMUTE_PS(vDot, _MM_SHUFFLE(0, 0, 0, 0));
}

inline float simd_dot_2(const float3& a, const float3& b) {
//	math::simd::Vector sa = math::simd::load_float4(a);
//	math::simd::Vector sb = math::simd::load_float4(b);

//	math::simd::Vector d = _mm_dp_ps(sa, sb, 0x77);

//	float x;
//	_mm_store_ss(&x, d);
//	return x;
	return math::dot(a, b);
}

void dot() {
	std::cout << "testing::simd::dot()" << std::endl;

	rnd::Generator rng(456, 90, 2123, 4598743);

	size_t num_values = 1024 * 1024 * (128 + 16);

	float3* vectors = new float3[num_values];

	for (size_t i = 0; i < num_values; ++i) {
		vectors[i] = float3(5.f - (10.f * rng.random_float()),
							5.f - (10.f * rng.random_float()),
							5.f - (10.f * rng.random_float()));
	}

	{
		std::cout << "math::dot()" << std::endl;
		std::cout << vectors[0] << " : " << math::dot(vectors[0], vectors[1]) << std::endl;

		auto start = std::chrono::high_resolution_clock::now();

		float result = 0.f;

		for (size_t i = 0, len = num_values - 1; i < len; ++i) {
			float3 a = vectors[i];
			float3 b = vectors[i + 1];
			float r = math::dot(a, b);
			result += r;
		}

		const auto duration = chrono::seconds_since(start);
		std::cout << result << " in " << string::to_string(duration) << " s" << std::endl;
		std::cout << std::endl;
	}

	{
		std::cout << "simd_dot_0()" << std::endl;
		std::cout << vectors[0] << " : " << simd_dot_0(vectors[0], vectors[1]) << std::endl;

		auto start = std::chrono::high_resolution_clock::now();

		float result = 0.f;

		for (size_t i = 0, len = num_values - 1; i < len; ++i) {
			float3 a = vectors[i];
			float3 b = vectors[i + 1];
			float r = simd_dot_0(a, b);
			result += r;
		}

		const auto duration = chrono::seconds_since(start);
		std::cout << result << " in " << string::to_string(duration) << " s" << std::endl;
		std::cout << std::endl;
	}

	{
		std::cout << "simd_dot_1()" << std::endl;
		std::cout << vectors[0] << " : " << simd_dot_1(vectors[0], vectors[1]) << std::endl;

		auto start = std::chrono::high_resolution_clock::now();

		float result = 0.f;

		for (size_t i = 0, len = num_values - 1; i < len; ++i) {
			float3 a = vectors[i];
			float3 b = vectors[i + 1];
			float r = simd_dot_1(a, b);
			result += r;
		}

		const auto duration = chrono::seconds_since(start);
		std::cout << result << " in " << string::to_string(duration) << " s" << std::endl;
		std::cout << std::endl;
	}

	{
		std::cout << "simd_dot_2()" << std::endl;
		std::cout << vectors[0] << " : " << simd_dot_2(vectors[0], vectors[1]) << std::endl;

		auto start = std::chrono::high_resolution_clock::now();

		float result = 0.f;

		for (size_t i = 0, len = num_values - 1; i < len; ++i) {
			float3 a = vectors[i];
			float3 b = vectors[i + 1];
			float r = simd_dot_2(a, b);
			result += r;
		}

		const auto duration = chrono::seconds_since(start);
		std::cout << result << " in " << string::to_string(duration) << " s" << std::endl;
		std::cout << std::endl;
	}

	{
		std::cout << "dotly()" << std::endl;
		std::cout << vectors[0] << " : " << dotly(vectors[0], vectors[1]) << std::endl;

		auto start = std::chrono::high_resolution_clock::now();

		float result = 0.f;

		for (size_t i = 0, len = num_values - 1; i < len; ++i) {
			float3 a = vectors[i];
			float3 b = vectors[i + 1];
			float r = dotly(a, b);
			result += r;
		}

		const auto duration = chrono::seconds_since(start);
		std::cout << result << " in " << string::to_string(duration) << " s" << std::endl;
		std::cout << std::endl;
	}

	delete[] vectors;
}

inline float3 simd_min(float3_p a, float3_p b) {
	math::simd::Vector sa = math::simd::load_float3(a);
	math::simd::Vector sb = math::simd::load_float3(b);

	float3 result;
	math::simd::store_float3_unsafe(result, _mm_min_ps(sa, sb));
	return result;
}

inline void simd_min2(float3& a, float3_p b) {
	math::simd::Vector sa = math::simd::load_float3(a);
	math::simd::Vector sb = math::simd::load_float3(b);

	math::simd::store_float3_unsafe(a, _mm_min_ps(sa, sb));
}

void minmax() {
	std::cout << "testing::simd::minmax()" << std::endl;

	rnd::Generator rng(456, 90, 2123, 4598743);

	size_t num_values = 1024 * 1024 * (128 + 64);

	float3* vectors = new float3[num_values];

	for (size_t i = 0; i < num_values; ++i) {
		vectors[i] = float3(5.f - (10.f * rng.random_float()),
							5.f - (10.f * rng.random_float()),
							5.f - (10.f * rng.random_float()));
	}

	{
		std::cout << "math::min()" << std::endl;

		auto start = std::chrono::high_resolution_clock::now();

		float3 result(1000.f, 1000.f, 1000.f);

		for (size_t i = 0, len = num_values; i < len; ++i) {
			result = math::min(result, vectors[i]);
		}

		const auto duration = chrono::seconds_since(start);
		std::cout << result << " in " << string::to_string(duration) << " s" << std::endl;
		std::cout << std::endl;
	}

	{
		std::cout << "simd_min()" << std::endl;

		auto start = std::chrono::high_resolution_clock::now();

		float3 result(1000.f, 1000.f, 1000.f);

		for (size_t i = 0, len = num_values; i < len; ++i) {
			result = simd_min(result, vectors[i]);
		}

		const auto duration = chrono::seconds_since(start);
		std::cout << result << " in " << string::to_string(duration) << " s" << std::endl;
		std::cout << std::endl;
	}

	{
		std::cout << "simd_min2()" << std::endl;

		auto start = std::chrono::high_resolution_clock::now();

		float3 result(1000.f, 1000.f, 1000.f);

		for (size_t i = 0, len = num_values; i < len; ++i) {
			simd_min2(result, vectors[i]);
		}

		const auto duration = chrono::seconds_since(start);
		std::cout << result << " in " << string::to_string(duration) << " s" << std::endl;
		std::cout << std::endl;
	}

	{
		std::cout << "math::min()" << std::endl;

		auto start = std::chrono::high_resolution_clock::now();

		float3 result(1000.f, 1000.f, 1000.f);

		for (size_t i = 0, len = num_values; i < len; ++i) {
			result = math::min(result, vectors[i]);
		}

		const auto duration = chrono::seconds_since(start);
		std::cout << result << " in " << string::to_string(duration) << " s" << std::endl;
		std::cout << std::endl;
	}

	delete[] vectors;
}

struct Union_vector;
using FUnion_vector = /*Union_vector;//*/const Union_vector&;

struct alignas(16) Union_vector {
	union {
		struct {
			float x, y, z, w;
		};

		// 4 instead of 3 in order to hide pad warning
		float v[4];
	};

	Union_vector() {}
	Union_vector(float x, float y, float z) : x(x), y(y), z(z), w(0.f)
	{
	//	v[3] = 0.f;
	}

	Union_vector operator+(FUnion_vector a) const {
		return Union_vector(x + a.x, y + a.y, z + a.z);
	}

	Union_vector operator/(float s) const {
		float is = 1.f / s;
		return Union_vector(x * is, y * is, z * is);
	}
};

Union_vector operator*(float s, FUnion_vector v) {
	return Union_vector(s * v.x, s * v.y, s * v.z);
}

float dot(FUnion_vector a, FUnion_vector b) {
	return a.x * b.x + a.y * b.y + a.z + b.z;
}

Union_vector normalized(FUnion_vector v) {
	return math::simd::rsqrt(dot(v, v)) * v;
}

struct Struct_vector;
using FStruct_vector = /*Struct_vector;//*/const Struct_vector&;

struct alignas(16) Struct_vector {
	float x, y, z, w;

	Struct_vector() {}
	Struct_vector(float x, float y, float z) : x(x), y(y), z(z)//, w(0.f)
	{
		w = 0.f;
	}

	Struct_vector operator+(FStruct_vector a) const {
		return Struct_vector(x + a.x, y + a.y, z + a.z);
	}

	Struct_vector operator/(float s) const {
		float is = 1.f / s;
		return Struct_vector(x * is, y * is, z * is);
	}
};

Struct_vector operator*(float s, FStruct_vector v) {
	return Struct_vector(s * v.x, s * v.y, s * v.z);
}

float dot(Struct_vector a, Struct_vector b) {
	return a.x * b.x + a.y * b.y + a.z + b.z;
}

Struct_vector normalized(FStruct_vector v) {
	return math::simd::rsqrt(dot(v, v)) * v;
}

struct Array_vector;
using FArray_vector = Array_vector;//*/const Array_vector&;

struct alignas(16) Array_vector {
	// 4 instead of 3 in order to hide pad warning
	float v[4];

	Array_vector() {}
	Array_vector(float x, float y, float z) : v{x, y, z, 0.f}
	{}

	float operator[](uint32_t i) const {
		return v[i];
	}

	float& operator[](uint32_t i) {
		return v[i];
	}

	Array_vector operator+(FArray_vector a) const {
		return Array_vector(v[0] + a[0], v[1] + a[1], v[2] + a[2]);
	}

	Array_vector operator/(float s) const {
		float is = 1.f / s;
		return Array_vector(v[0] * is, v[1] * is, v[2] * is);
	}
};

Array_vector operator*(float s, FArray_vector v) {
	return Array_vector(s * v[0], s * v[1], s * v[2]);
}

float dot(FArray_vector a, FArray_vector b) {
	return a[0] * b[0] + a[1] * b[1] + a[2] + b[2];
}

Array_vector normalized(FArray_vector v) {
	return math::simd::rsqrt(dot(v, v)) * v;
}

void test_union_vector(Union_vector* uvecs, size_t num_values) {
	std::cout << "Union vector" << std::endl;

	auto start = std::chrono::high_resolution_clock::now();

	Union_vector result(0.f, 0.f, 0.f);

	for (size_t i = 0; i < num_values; ++i) {
		Union_vector v = uvecs[i];
		float d = dot(v, v);
		Union_vector t = (v.y * (result + v)) / (d + 0.1f);
		Union_vector w = (v.x * (result + v)) / (d + 0.3f);
		Union_vector n = /*normalized*/((v + t) + ((d * v) + (d * t)));
		result = n + ((w + t) + (d * w));
	}

	const auto duration = chrono::seconds_since(start);
	std::cout << "[" << result.x << ", " << result.y << ", " << result.z << "] in " << string::to_string(duration) << " s" << std::endl;
}

void test_struct_vector(Struct_vector* svecs, size_t num_values) {
	std::cout << "Struct vector" << std::endl;

	auto start = std::chrono::high_resolution_clock::now();

	Struct_vector result(0.f, 0.f, 0.f);

	for (size_t i = 0; i < num_values; ++i) {
		Struct_vector v = svecs[i];
		float d = dot(v, v);
		Struct_vector t = (v.y * (result + v)) / (d + 0.1f);
		Struct_vector w = (v.x * (result + v)) / (d + 0.3f);
		Struct_vector n = /*normalized*/((v + t) + ((d * v) + (d * t)));
		result = n + ((w + t) + (d * w));
	}

	const auto duration = chrono::seconds_since(start);
	std::cout << "[" << result.x << ", " << result.y << ", " << result.z << "] in " << string::to_string(duration) << " s" << std::endl;
}

void test_array_vector(Array_vector* vecs, size_t num_values) {
	std::cout << "Array vector" << std::endl;

	auto start = std::chrono::high_resolution_clock::now();

	Array_vector result(0.f, 0.f, 0.f);

	for (size_t i = 0; i < num_values; ++i) {
		Array_vector v = vecs[i];
		float d = dot(v, v);
		Array_vector t = (v[1] * (result + v)) / (d + 0.1f);
		Array_vector w = (v[0] * (result + v)) / (d + 0.3f);
		Array_vector n = /*normalized*/((v + t) + ((d * v) + (d * t)));
		result = n + ((w + t) + (d * w));
	}

	const auto duration = chrono::seconds_since(start);
	std::cout << "[" << result[0] << ", " << result[1] << ", " << result[2] << "] in " << string::to_string(duration) << " s" << std::endl;
}

void unions() {
	std::cout << "testing::simd::unions()" << std::endl;

	rnd::Generator rng(456, 90, 2123, 4598743);

	size_t num_values = 1024 * 1024 * 128;// * (128 + 32);

	Union_vector*  uvecs = new Union_vector[num_values];
	Struct_vector* svecs = new Struct_vector[num_values];
	Array_vector*  avecs = new Array_vector[num_values];

	for (size_t i = 0; i < num_values; ++i) {
		float x = 0.000025f * rng.random_float();
		float y = 0.000025f * rng.random_float();
		float z = 0.000025f * rng.random_float();

		uvecs[i] = Union_vector(x, y, z);
		svecs[i] = Struct_vector(x, y, z);
		avecs[i] = Array_vector(x, y, z);
	}

	test_union_vector(uvecs, num_values);
	test_struct_vector(svecs, num_values);
	test_array_vector(avecs, num_values);
	test_union_vector(uvecs, num_values);
	test_struct_vector(svecs, num_values);
	test_array_vector(avecs, num_values);

	delete[] avecs;
	delete[] svecs;
	delete[] uvecs;
}

}}
