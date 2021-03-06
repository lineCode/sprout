#include "volumetric_octree_builder.hpp"
#include "base/math/vector3.inl"
#include "image/texture/texture.hpp"

namespace scene::material::volumetric {

Octree_builder::Build_node::~Build_node() {
    for (uint32_t i = 0; i < 8; ++i) {
        delete children[i];
    }
}

void Octree_builder::build(Gridtree& tree, Texture const& texture, CM const& idata) {
    static int32_t constexpr cell_max_dimension = 36;

    int3 const d    = texture.dimensions_3();
    int3 const cell = math::min(d, cell_max_dimension);

    int3 num_cells = d / cell;

    num_cells += math::min(d - num_cells * cell, 1);

    uint32_t const cell_len = static_cast<uint32_t>(num_cells[0] * num_cells[1] * num_cells[2]);

    num_nodes_ = cell_len;
    num_data_  = 0;

    Build_node* grid = new Build_node[cell_len];

    Build_node* node = grid;
    for (int32_t z = 0; z < num_cells[2]; ++z) {
        for (int32_t y = 0; y < num_cells[1]; ++y) {
            for (int32_t x = 0; x < num_cells[0]; ++x, ++node) {
                int3 const min = int3(x, y, z) * cell;
                int3 const max = math::min(min + cell, d);

                Box const box{{min, max}};
                split(node, box, texture, idata, 0, 3);
            }
        }
    }

    tree.set_dimensions(d, cell, num_cells);

    nodes_ = tree.allocate_nodes(num_nodes_);

    data_ = tree.allocate_data(num_data_);

    uint32_t next = cell_len;
    uint32_t data = 0;

    for (uint32_t i = 0; i < cell_len; ++i) {
        serialize(&grid[i], i, next, data);
    }

    delete[] grid;
}

void Octree_builder::split(Build_node* node, Box const& box, Texture const& texture,
                           CM const& idata, uint32_t depth, uint32_t max_depth) {
    // Include 1 additional voxel on each border to account for filtering
    int3 const minb = math::max(box.bounds[0] - 1, 0);
    int3 const maxb = math::min(box.bounds[1] + 1, texture.dimensions_3());

    float min_density = 1.f;
    float max_density = 0.f;
    for (int32_t z = minb[2], mz = maxb[2]; z < mz; ++z) {
        for (int32_t y = minb[1], my = maxb[1]; y < my; ++y) {
            for (int32_t x = minb[0], mx = maxb[0]; x < mx; ++x) {
                float const density = texture.at_1(x, y, z);

                min_density = std::min(density, min_density);
                max_density = std::max(density, max_density);
            }
        }
    }

    float const minorant_mu_a = min_density * idata.minorant_mu_a;
    float const minorant_mu_s = min_density * idata.minorant_mu_s;
    float const majorant_mu_t = max_density * idata.majorant_mu_t;

    float const diff = majorant_mu_t - (minorant_mu_a + minorant_mu_s);

    int3 const half = (box.bounds[1] - box.bounds[0]) / 2;

    if (max_depth == depth || diff < 0.1f || math::any_less(half, 3)) {
        for (uint32_t i = 0; i < 8; ++i) {
            node->children[i] = nullptr;
        }

        if (0.f == diff) {
            node->data.minorant_mu_a = minorant_mu_a;
            node->data.minorant_mu_s = minorant_mu_s;
            node->data.minorant_mu_t = minorant_mu_a + minorant_mu_s;
            node->data.majorant_mu_t = majorant_mu_t;
        } else {
            // Without an epsilon the sampled extinction coefficient can sometimes
            // be a tiny bit larger than the majorant computed here.
            // Supposedly due to floating point imprecision.
            static float constexpr mt_epsilon = 0.005f;

            node->data.minorant_mu_a = std::max(minorant_mu_a - mt_epsilon, 0.f);
            node->data.minorant_mu_s = std::max(minorant_mu_s - mt_epsilon, 0.f);
            node->data.minorant_mu_t = node->data.minorant_mu_a + node->data.minorant_mu_s;
            node->data.majorant_mu_t = 0.f == majorant_mu_t ? 0.f : majorant_mu_t + mt_epsilon;
        }

        ++num_data_;

        return;
    }

    ++depth;

    int3 const center = box.bounds[0] + half;

    {
        Box const sub{{box.bounds[0], center}};

        node->children[0] = new Build_node;
        split(node->children[0], sub, texture, idata, depth, max_depth);
    }

    {
        Box const sub{{int3(center[0], box.bounds[0][1], box.bounds[0][2]),
                       int3(box.bounds[1][0], center[1], center[2])}};

        node->children[1] = new Build_node;
        split(node->children[1], sub, texture, idata, depth, max_depth);
    }

    {
        Box const sub{{int3(box.bounds[0][0], center[1], box.bounds[0][2]),
                       int3(center[0], box.bounds[1][1], center[2])}};

        node->children[2] = new Build_node;
        split(node->children[2], sub, texture, idata, depth, max_depth);
    }

    {
        Box const sub{{int3(center[0], center[1], box.bounds[0][2]),
                       int3(box.bounds[1][0], box.bounds[1][1], center[2])}};

        node->children[3] = new Build_node;
        split(node->children[3], sub, texture, idata, depth, max_depth);
    }

    {
        Box const sub{{int3(box.bounds[0][0], box.bounds[0][1], center[2]),
                       int3(center[0], center[1], box.bounds[1][2])}};

        node->children[4] = new Build_node;
        split(node->children[4], sub, texture, idata, depth, max_depth);
    }

    {
        Box const sub{{int3(center[0], box.bounds[0][1], center[2]),
                       int3(box.bounds[1][0], center[1], box.bounds[1][2])}};

        node->children[5] = new Build_node;
        split(node->children[5], sub, texture, idata, depth, max_depth);
    }

    {
        Box const sub{{int3(box.bounds[0][0], center[1], center[2]),
                       int3(center[0], box.bounds[1][1], box.bounds[1][2])}};

        node->children[6] = new Build_node;
        split(node->children[6], sub, texture, idata, depth, max_depth);
    }

    {
        Box const sub{{center, box.bounds[1]}};

        node->children[7] = new Build_node;
        split(node->children[7], sub, texture, idata, depth, max_depth);
    }

    num_nodes_ += 8;
}

void Octree_builder::serialize(Build_node* node, uint32_t current, uint32_t& next, uint32_t& data) {
    auto& n = nodes_[current];

    if (node->children[0]) {
        n.set_children(next);

        current = next;
        next += 8;

        for (uint32_t i = 0; i < 8; ++i) {
            serialize(node->children[i], current + i, next, data);
        }
    } else {
        n.set_data(data);

        data_[data] = node->data;

        ++data;
    }
}

}  // namespace scene::material::volumetric
