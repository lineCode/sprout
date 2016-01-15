#pragma once

#include "triangle_bvh_builder_sah.hpp"
#include "triangle_bvh_builder_base.inl"
#include "triangle_bvh_tree.inl"
#include "triangle_bvh_helper.hpp"
#include "scene/shape/node_stack.inl"
#include "scene/shape/triangle/triangle_primitive.hpp"
#include "base/math/vector.inl"
#include "base/math/plane.inl"
#include "base/math/bounding/aabb.inl"

namespace scene { namespace shape { namespace triangle { namespace bvh {

template<typename Data>
void Builder_SAH::build(Tree<Data>& tree,
						const std::vector<Index_triangle>& triangles,
						const std::vector<Vertex>& vertices,
						uint32_t max_primitives,
						thread::Pool& thread_pool) {
	std::vector<uint32_t> primitive_indices(triangles.size());
	std::vector<math::aabb> primitive_bounds(triangles.size());

	math::aabb aabb = math::aabb::empty();

	for (uint32_t i = 0, len = static_cast<uint32_t>(triangles.size()); i < len; ++i) {
		primitive_indices[i] = i;

		auto& a = vertices[triangles[i].a].p;
		auto& b = vertices[triangles[i].b].p;
		auto& c = vertices[triangles[i].c].p;

		math::float3 min = triangle_min(a, b, c);
		math::float3 max = triangle_max(a, b, c);

		primitive_bounds[i] = math::aabb(min, max);

		aabb.merge_assign(primitive_bounds[i]);
	}

	tree.allocate_triangles(static_cast<uint32_t>(triangles.size()));

	Build_node root;
	split(&root,
		  primitive_indices.begin(), primitive_indices.end(),
		  aabb, triangles, vertices, primitive_bounds, max_primitives, thread_pool, tree);

	num_nodes_ = 1;
	root.num_sub_nodes(num_nodes_);

	nodes_ = &tree.allocate_nodes(num_nodes_);

	current_node_ = 0;
	serialize(&root);
}

template<typename Data>
void Builder_SAH::split(Build_node* node,
						index begin, index end,
						const math::aabb& aabb,
						const std::vector<Index_triangle>& triangles,
						const std::vector<Vertex>& vertices,
						const std::vector<math::aabb>& triangle_bounds,
						uint32_t max_primitives,
						thread::Pool& thread_pool,
						Tree<Data>& tree) {
	node->aabb = aabb;

	uint32_t num_primitives = static_cast<uint32_t>(std::distance(begin, end));

	if (num_primitives <= max_primitives) {
		assign(node, begin, end, triangles, vertices, tree);
	} else {
		Split_candidate sp = splitting_plane(begin, end, aabb, triangle_bounds, thread_pool);

		if (static_cast<float>(num_primitives) <= sp.cost()) {
			assign(node, begin, end, triangles, vertices, tree);
		} else {
			node->axis = sp.axis();

			index pids1_begin = std::partition(begin, end,
				[&sp, &triangle_bounds](uint32_t pi) { return sp.behind(triangle_bounds[pi].max()); });

			if (begin == pids1_begin || end == pids1_begin) {
				// This can happen if we didn't find a good splitting plane.
				// It means every triangle was (partially) on the same side of the plane.

				assign(node, begin, end, triangles, vertices, tree);
			} else {
				node->children[0] = new Build_node;
				split(node->children[0], begin, pids1_begin, sp.aabb_0(), triangles, vertices, triangle_bounds,
					  max_primitives, thread_pool, tree);

				node->children[1] = new Build_node;
				split(node->children[1], pids1_begin, end, sp.aabb_1(), triangles, vertices, triangle_bounds,
					  max_primitives, thread_pool, tree);
			}
		}
	}
}

}}}}