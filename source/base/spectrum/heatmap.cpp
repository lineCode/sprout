#include "heatmap.hpp"
#include <iterator>
#include "math/vector3.inl"

namespace spectrum {

float3 heatmap(float x) {
    // The last color is intentionally present twice, as a convenient way to catch (1.f == x) cases.
    static constexpr float3 colors[] = {{0.f, 0.f, 0.f}, {0.f, 0.f, 1.f}, {0.f, 1.f, 0.f},
                                        {1.f, 1.f, 0.f}, {1.f, 0.f, 0.f}, {1.f, 0.f, 0.f}};

    static int32_t constexpr max_color = static_cast<int32_t>(std::size(colors) - 2);

    int32_t const id = static_cast<int32_t>(x * static_cast<float>(max_color));
    float const   ip = x * static_cast<float>(max_color) - static_cast<float>(id);

    return math::lerp(colors[id], colors[id + 1], ip);
}

}  // namespace spectrum
