#pragma once

#include "triangle_bvh_builder.hpp"
#include "base/math/plane.hpp"
#include "base/math/vector.hpp"

namespace scene { namespace shape { namespace triangle { namespace bvh {

class Split_candidate {
public:

	Split_candidate(const math::plane& plane, uint8_t axis);

	typedef std::vector<uint32_t>::iterator index;

	Split_candidate(uint8_t bb_axis, uint8_t split_axis, const math::float3& p,
					index begin, index end,
					const std::vector<Index_triangle>& triangles,
					const std::vector<Vertex>& vertices);

	uint64_t key() const;

	const math::plane& plane() const;

	uint8_t axis() const;

private:

	uint64_t key_;

	math::plane plane_;

	uint8_t axis_;
};

class Builder_SUH : private Builder  {
public:

	template<typename Data>
	void build(Tree<Data>& tree,
			   const std::vector<Index_triangle>& triangles,
			   const std::vector<Vertex>& vertices,
			   uint32_t max_primitives);

private:

	std::vector<Split_candidate> split_candidates_;

	template<typename Data>
	void split(Build_node* node,
			   index begin, index end,
			   const std::vector<Index_triangle>& triangles,
			   const std::vector<Vertex>& vertices,
			   uint32_t max_primitives, uint32_t depth,
			   Tree<Data>& tree);

	Split_candidate splitting_plane(const math::aabb& aabb,
									index begin, index end,
									const std::vector<Index_triangle>& triangles,
									const std::vector<Vertex>& vertices);
};

}}}}