#pragma once

#include "material/material_sample_cache.inl"
#include "scene_worker.hpp"

namespace scene {

template <typename T>
T& Worker::sample(uint32_t depth) const noexcept {
    return sample_cache_.get<T>(depth);
}

}  // namespace scene
