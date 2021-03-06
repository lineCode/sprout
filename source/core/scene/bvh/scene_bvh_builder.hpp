#ifndef SU_CORE_SCENE_BVH_BUILDER_HPP
#define SU_CORE_SCENE_BVH_BUILDER_HPP

#include <cstddef>
#include <vector>
#include "base/math/aabb.hpp"
#include "base/math/plane.hpp"
#include "scene_bvh_split_candidate.hpp"

namespace scene::bvh {

class Node;
template <typename T>
struct Tree;

template <typename T>
class Builder {
  public:
    Builder() noexcept;

    ~Builder() noexcept;

    void build(Tree<T>& tree, std::vector<T*>& finite_props) noexcept;

  private:
    struct Build_node {
        Build_node() noexcept = default;

        ~Build_node() noexcept;

        void clear() noexcept;

        math::AABB aabb;

        uint8_t axis;

        uint32_t offset;
        uint32_t props_end;

        Build_node* children[2] = {nullptr, nullptr};
    };

    using index = typename std::vector<T*>::iterator;

    void split(Build_node* node, index begin, index end, uint32_t max_shapes,
               std::vector<T*>& out_props) noexcept;

    Split_candidate<T> splitting_plane(math::AABB const& aabb, index begin, index end) noexcept;

    void serialize(Build_node* node) noexcept;

    Node& new_node() noexcept;

    uint32_t current_node_index() const noexcept;

    static void assign(Build_node* node, index begin, index end,
                       std::vector<T*>& out_data) noexcept;

    static math::AABB aabb(index begin, index end) noexcept;

    std::vector<Split_candidate<T>> split_candidates_;

    Build_node* root_;

    uint32_t num_nodes_;
    uint32_t current_node_;

    Node* nodes_;
};

}  // namespace scene::bvh

#endif
