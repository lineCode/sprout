#include "triangle_bvh_builder_base.hpp"
#include "triangle_bvh_tree.inl"
#include "triangle_bvh_helper.hpp"
#include "scene/shape/triangle/triangle_primitive.hpp"
#include "scene/shape/geometry/vertex.hpp"
#include "base/math/vector.inl"
#include "base/math/bounding/aabb.inl"

namespace scene { namespace shape { namespace triangle { namespace bvh {

Build_node::Build_node() : start_index(0), end_index(0) {
	children[0] = nullptr;
	children[1] = nullptr;
}

Build_node::~Build_node() {
	delete children[0];
	delete children[1];
}

void Build_node::num_sub_nodes(uint32_t& count) {
	if (children[0]) {
		count += 2;

		children[0]->num_sub_nodes(count);
		children[1]->num_sub_nodes(count);
	}
}

void Builder_base::serialize(Build_node* node) {
	auto& n = new_node();
	n.aabb = node->aabb;
	/*
	n.start_index = node->start_index;
	n.end_index = node->end_index;

	if (node->children[0]) {
		serialize(node->children[0]);

		n.set_right_child(current_node_index());

		serialize(node->children[1]);

		n.set_has_children(true);

		// axis and start_index share the same memory, so only set this if node contains no triangles
		n.set_axis(node->axis);
	}
	*/

	if (node->children[0]) {
		serialize(node->children[0]);

		n.second_child_index = current_node_index();

		serialize(node->children[1]);

		n.axis = node->axis;
		n.num_primitives = 0;
	} else {
		n.primitive_offset = node->start_index;
		n.num_primitives = static_cast<uint8_t>(node->end_index - node->start_index);
	}
}

Node& Builder_base::new_node() {
	return (*nodes_)[current_node_++];
}

uint32_t Builder_base::current_node_index() const {
	return current_node_;
}

math::aabb Builder_base::submesh_aabb(index begin, index end,
									  const std::vector<Index_triangle>& triangles,
									  const std::vector<Vertex>& vertices) {

	float max_float = std::numeric_limits<float>::max();
	math::float3 min( max_float,  max_float,  max_float);
	math::float3 max(-max_float, -max_float, -max_float);

	for (index i = begin; i != end; ++i) {
		auto& t = triangles[*i];
		min = triangle_min(vertices[t.a].p, vertices[t.b].p, vertices[t.c].p, min);
		max = triangle_max(vertices[t.a].p, vertices[t.b].p, vertices[t.c].p, max);
	}

//	constexpr float epsilon = 0.000000001f;

//	max.x += epsilon;
//	max.y += epsilon;
//	max.z += epsilon;

	return math::aabb(min, max);
}

}}}}
