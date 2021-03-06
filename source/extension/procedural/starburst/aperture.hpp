#pragma once

#include <vector>
#include "base/math/vector3.hpp"

namespace procedural {
namespace starburst {

class Aperture {
  public:
    Aperture(uint32_t num_blades, float roundness, float rotation);

    float evaluate(float2 p, float radius) const;

  private:
    std::vector<float3> blades_;

    float roundness_;
};

}  // namespace starburst
}  // namespace procedural
