#include "volumetric_octree.hpp"
#include "base/math/aabb.inl"
#include "base/math/ray.inl"
#include "base/math/vector3.inl"
#include "base/memory/align.hpp"
#include "scene/material/material.hpp"

#include "base/debug/assert.hpp"

namespace scene::material::volumetric {

Gridtree::Gridtree() noexcept : num_nodes_(0), nodes_(nullptr), num_data_(0), data_(nullptr) {}

Gridtree::~Gridtree() noexcept {
    memory::free_aligned(data_);
    memory::free_aligned(nodes_);
}

Node* Gridtree::allocate_nodes(uint32_t num_nodes) noexcept {
    if (num_nodes != num_nodes_) {
        num_nodes_ = num_nodes;

        memory::free_aligned(nodes_);
        nodes_ = memory::allocate_aligned<Node>(num_nodes);
    }

    return nodes_;
}

CM* Gridtree::allocate_data(uint32_t num_data) noexcept {
    if (num_data != num_data_) {
        num_data_ = num_data;

        memory::free_aligned(data_);
        data_ = memory::allocate_aligned<CM>(num_data);
    }

    return data_;
}

void Gridtree::set_dimensions(int3 const& dimensions, int3 const& cell_dimensions,
                              int3 const& num_cells) noexcept {
    dimensions_ = dimensions;

    num_cells_ = num_cells;

    cell_dimensions_ = float3(cell_dimensions) / float3(dimensions);

    factor_ = float3(dimensions) / float3(cell_dimensions * num_cells);
}

bool Gridtree::is_valid() const noexcept {
    return nullptr != nodes_;
}

bool Gridtree::intersect(math::Ray& ray, CM& data) const noexcept {
    float3 const p = ray.point(ray.min_t);

    if (math::any_greater(p, 1.f)) {
        return false;
    }

    int3 const v = int3(float3(num_cells_) * (factor_ * p));

    if (math::any_less(v, 0) || math::any_greater_equal(v, num_cells_)) {
        return false;
    }

    uint32_t index = static_cast<uint32_t>((v[2] * num_cells_[1] + v[1]) * num_cells_[0] + v[0]);

    math::AABB box;
    box.bounds[0] = float3(v) * cell_dimensions_;
    box.bounds[1] = math::min(box.bounds[0] + cell_dimensions_, 1.f);

    for (;;) {
        auto const& node = nodes_[index];

        if (!node.has_children()) {
            break;
        }

        index = node.index();

        float3 const half = box.halfsize();

        float3 const center = box.bounds[0] + half;

        if (p[0] < center[0]) {
            box.bounds[1][0] = center[0];
        } else {
            box.bounds[0][0] = center[0];
            index += 1;
        }

        if (p[1] < center[1]) {
            box.bounds[1][1] = center[1];
        } else {
            box.bounds[0][1] = center[1];
            index += 2;
        }

        if (p[2] < center[2]) {
            box.bounds[1][2] = center[2];
        } else {
            box.bounds[0][2] = center[2];
            index += 4;
        }
    }

    float hit_t;
    if (box.intersect_inside(ray, hit_t)) {
        if (ray.max_t > hit_t) {
            ray.max_t = hit_t;
        }
    } else {
        ray.max_t = ray.min_t;
        return false;
    }

    data = data_[nodes_[index].index()];

    return true;
}

size_t Gridtree::num_bytes() const noexcept {
    return sizeof(*this) + num_nodes_ * sizeof(Node) + num_data_ * sizeof(CM);
}

}  // namespace scene::material::volumetric
