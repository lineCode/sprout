#ifndef SU_CORE_SCENE_SHAPE_TRIANGLE_BVH_TREE_HPP
#define SU_CORE_SCENE_SHAPE_TRIANGLE_BVH_TREE_HPP

#include "scene/material/material.hpp"
#include "base/math/aabb.hpp"
#include "base/math/vector3.hpp"

namespace math { struct Ray; }

namespace scene {

namespace bvh { class Node; }

class Worker;

namespace shape {

class Node_stack;
struct Vertex;

namespace triangle {

struct Intersection;
struct Index_triangle;
struct Triangle;
struct Position_triangle;
struct Data_triangle;

namespace bvh {

template<typename Data>
class Tree  {

public:

	Tree();
	~Tree();

	using Node = scene::bvh::Node;

	Node* allocate_nodes(uint32_t num_nodes);

	math::AABB aabb() const;

	uint32_t num_parts() const;

	uint32_t num_triangles() const;
	uint32_t num_triangles(uint32_t part) const;

	uint32_t current_triangle() const;

	bool intersect(math::Ray& ray, Node_stack& node_stack, Intersection& intersection) const;

	bool intersect(math::Ray& ray, Node_stack& node_stack) const;

	bool intersect(FVector ray_origin, FVector ray_direction, FVector ray_inv_direction,
				   FVector ray_min_t, Vector& ray_max_t, uint32_t ray_signs[4],
				   Node_stack& node_stack, Intersection& intersection) const;

	bool intersect(FVector ray_origin, FVector ray_direction, FVector ray_inv_direction,
				   FVector ray_min_t, Vector& ray_max_t, uint32_t ray_signs[4],
				   Node_stack& node_stack) const;

	bool intersect_p(math::Ray const& ray, Node_stack& node_stack) const;

	bool intersect_p(FVector ray_origin, FVector ray_direction, FVector ray_inv_direction,
					 FVector ray_min_t, FVector ray_max_t, uint32_t ray_signs[4],
					 Node_stack& node_stack) const;

	float opacity(math::Ray& ray, float time, Materials const& materials,
				  material::Sampler_settings::Filter filter, Worker const& worker) const;

	float3 absorption(math::Ray& ray, float time, Materials const& materials,
					  material::Sampler_settings::Filter filter, Worker const& worker) const;

	void interpolate_triangle_data(uint32_t index, float2 uv,
								   float3& n, float3& t, float2& tc) const;

	void interpolate_triangle_data(FVector u, FVector v, uint32_t index,
								   float3& n, float3& t, float2& tc) const;

	void interpolate_triangle_data(FVector u, FVector v, uint32_t index,
								   Vector& n, Vector& t, float2& tc) const;


    float2 interpolate_triangle_uv(uint32_t index, float2 uv) const;

	float2 interpolate_triangle_uv(FVector u, FVector v, uint32_t index) const;

	float    triangle_bitangent_sign(uint32_t index) const;
    uint32_t triangle_material_index(uint32_t index) const;

	float3 triangle_normal(uint32_t index) const;
	Vector triangle_normal_v(uint32_t index) const;

	float triangle_area(uint32_t index) const;
	float triangle_area(uint32_t index, f_float3 scale) const;

	void sample(uint32_t index, float2 r2, float3& p, float3& n, float2& tc) const;
	void sample(uint32_t index, float2 r2, float3& p, float2& tc) const;
	void sample(uint32_t index, float2 r2, float3& p) const;

	void allocate_parts(uint32_t num_parts);

	void allocate_triangles(uint32_t num_triangles, const std::vector<Vertex>& vertices);

	void add_triangle(uint32_t a, uint32_t b, uint32_t c, uint32_t material_index,
					  const std::vector<Vertex>& vertices);

	size_t num_bytes() const;

private:

	uint32_t num_nodes_;
	Node*	 nodes_;

	uint32_t  num_parts_;
	uint32_t* num_part_triangles_;

    Data data_;
};

}}}}

#endif
