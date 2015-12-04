#pragma once

#include "base/math/vector.hpp"
#include <vector>

namespace scene { namespace shape {

struct Vertex;

namespace triangle { namespace bvh {

template<typename Triangle>
class Data_generic {
public:

	~Data_generic();

	uint32_t num_triangles() const;

	bool intersect(uint32_t index, math::Oray& ray, math::float2& uv) const;

	bool intersect_p(uint32_t index, const math::Oray& ray) const;

	void interpolate_data(uint32_t index, math::float2 uv,
						  math::float3& n, math::float3& t, math::float2& tc) const;

	math::float2 interpolate_uv(uint32_t index, math::float2 uv) const;

	float    bitangent_sign(uint32_t index) const;
	uint32_t material_index(uint32_t index) const;

	math::float3 normal(uint32_t index) const;

	float area(uint32_t index) const;
	float area(uint32_t index, const math::float3& scale) const;

	void sample(uint32_t index, math::float2 r2, math::float3& p, math::float3& n, math::float2& tc) const;
	void sample(uint32_t index, math::float2 r2, math::float3& p, math::float2& tc) const;
	void sample(uint32_t index, math::float2 r2, math::float3& p) const;

	void allocate_triangles(uint32_t num_triangles);

	void add_triangle(const Vertex& a, const Vertex& b, const Vertex& c, uint32_t material_index);

private:

	std::vector<Triangle> triangles_;
};

}}}}