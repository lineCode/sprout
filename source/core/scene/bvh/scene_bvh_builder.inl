#ifndef SU_CORE_SCENE_BVH_BUILDER_INL
#define SU_CORE_SCENE_BVH_BUILDER_INL

#include "base/math/aabb.inl"
#include "base/math/plane.inl"
#include "scene_bvh_builder.hpp"
#include "scene_bvh_node.inl"
#include "scene_bvh_split_candidate.inl"
#include "scene_bvh_tree.inl"

namespace scene::bvh {

template <typename T>
Builder<T>::Builder() noexcept : root_(new Build_node) {
    root_->clear();
}

template <typename T>
Builder<T>::~Builder() noexcept {
    delete root_;
}

template <typename T>
void Builder<T>::build(Tree<T>& tree, std::vector<T*>& finite_props) noexcept {
    tree.clear();

    tree.data_.reserve(finite_props.size());

    if (finite_props.empty()) {
        nodes_ = tree.allocate_nodes(0);
    } else {
        num_nodes_ = 1;
        split(root_, finite_props.begin(), finite_props.end(), 4, tree.data_);

        nodes_ = tree.allocate_nodes(num_nodes_);

        current_node_ = 0;
        serialize(root_);
    }

    tree.aabb_ = root_->aabb;

    root_->clear();
}

template <typename T>
Builder<T>::Build_node::~Build_node() noexcept {
    delete children[0];
    delete children[1];
}

template <typename T>
void Builder<T>::Build_node::clear() noexcept {
    delete children[0];
    children[0] = nullptr;
    delete children[1];
    children[1] = nullptr;

    props_end = 0;
    offset    = 0;

    // This size will be used even if there are only infinite props in the scene.
    // It is an arbitrary size that will be used to calculate the power of some lights.
    aabb = math::AABB(float3(-1.f), float3(1.f));
}

template <typename T>
void Builder<T>::split(Build_node* node, index begin, index end, uint32_t max_shapes,
                       std::vector<T*>& out_data) noexcept {
    node->aabb = aabb(begin, end);

    if (static_cast<uint32_t>(std::distance(begin, end)) <= max_shapes) {
        assign(node, begin, end, out_data);
    } else {
        Split_candidate<T> sp = splitting_plane(node->aabb, begin, end);
        node->axis            = sp.axis();

        index props1_begin = std::partition(begin, end, [&sp](T* b) {
            bool const mib = math::plane::behind(sp.plane(), b->aabb().min());
            bool const mab = math::plane::behind(sp.plane(), b->aabb().max());

            return mib && mab;
        });

        if (begin == props1_begin) {
            assign(node, props1_begin, end, out_data);
        } else {
            node->children[0] = new Build_node;
            split(node->children[0], begin, props1_begin, max_shapes, out_data);

            node->children[1] = new Build_node;
            split(node->children[1], props1_begin, end, max_shapes, out_data);

            num_nodes_ += 2;
        }
    }
}

template <typename T>
Split_candidate<T> Builder<T>::splitting_plane(math::AABB const& /*aabb*/, index begin,
                                               index end) noexcept {
    split_candidates_.clear();

    float3 average = float3::identity();

    for (index i = begin; i != end; ++i) {
        average += (*i)->aabb().position();
    }

    average /= static_cast<float>(std::distance(begin, end));

    split_candidates_.push_back(Split_candidate<T>(0, average, begin, end));

    split_candidates_.push_back(Split_candidate<T>(1, average, begin, end));

    split_candidates_.push_back(Split_candidate<T>(2, average, begin, end));

    std::sort(
        split_candidates_.begin(), split_candidates_.end(),
        [](const Split_candidate<T>& a, const Split_candidate<T>& b) { return a.key() < b.key(); });

    return split_candidates_[0];
}

template <typename T>
void Builder<T>::serialize(Build_node* node) noexcept {
    auto& n = new_node();
    n.set_aabb(node->aabb.min().v, node->aabb.max().v);

    if (node->children[0]) {
        serialize(node->children[0]);

        n.set_split_node(current_node_index(), node->axis);

        serialize(node->children[1]);
    } else {
        uint8_t const num_primitives = static_cast<uint8_t>(node->props_end - node->offset);
        n.set_leaf_node(node->offset, num_primitives);
    }
}

template <typename T>
bvh::Node& Builder<T>::new_node() noexcept {
    return nodes_[current_node_++];
}

template <typename T>
uint32_t Builder<T>::current_node_index() const noexcept {
    return current_node_;
}

template <typename T>
void Builder<T>::assign(Build_node* node, index begin, index end,
                        std::vector<T*>& out_props) noexcept {
    node->offset = static_cast<uint32_t>(out_props.size());

    for (index i = begin; i != end; ++i) {
        out_props.push_back(*i);
    }

    node->props_end = static_cast<uint32_t>(out_props.size());
}

template <typename T>
math::AABB Builder<T>::aabb(index begin, index end) noexcept {
    math::AABB aabb = math::AABB::empty();

    for (index i = begin; i != end; ++i) {
        aabb.merge_assign((*i)->aabb());
    }

    return aabb;
}

}  // namespace scene::bvh

#endif
