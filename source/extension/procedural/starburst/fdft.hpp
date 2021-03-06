#pragma once

#include "base/math/math.hpp"
#include "base/math/vector2.inl"
#include "base/thread/thread_pool.hpp"
#include "core/image/typed_image.hpp"

namespace procedural {
namespace starburst {

static inline float2 sqrtc(float2 c) {
    float l = math::length(c);
    return 0.7071067f * float2(std::sqrt(l + c[0]), std::sqrt(l - c[0]) * std::copysign(1.f, c[1]));
}

static inline float2 mulc(float2 a, float2 b) {
    return float2(a[0] * b[0] - a[1] * b[1], a[0] * b[1] + a[1] * b[0]);
}

static inline float2 mulc_cos_sin(float a, float2 cos_sin) {
    return float2(a * cos_sin[0], a * cos_sin[1]);
}

static inline float2 mulc_cos_sin(float2 a, float2 cos_sin) {
    return float2(a[0] * cos_sin[0] - a[1] * cos_sin[1], a[0] * cos_sin[1] + a[1] * cos_sin[0]);
}

static inline float2 mulc(float2 a, float t) {
    float c = std::cos(t);
    float s = std::sin(t);

    return float2(a[0] * c - a[1] * s, a[0] * s + a[1] * c);
}

static float map(int32_t b, float tc, int2& x_x1) {
    float const u = tc - 0.5f;

    float const fu = std::floor(u);

    int32_t const x = static_cast<int32_t>(fu);

    x_x1[0] = x < 0 ? 0 : x;
    x_x1[1] = x >= b ? b : x + 1;

    return u - fu;
}

template <typename T>
static T sample(const T* source, int32_t w, int32_t b, float tc, int32_t y) {
    int2        x_x1;
    float const s  = map(b, tc, x_x1);
    float const _s = 1.f - s;

    int32_t const y0 = w * y;
    T             c0 = source[y0 + x_x1[0]];
    T             c1 = source[y0 + x_x1[1]];

    return _s * c0 + s * c1;
}

class Row {
  public:
    Row(uint32_t width, float alpha, thread::Pool& pool)
        : width_(width),
          num_samples_(uint32_t(float(width) * std::floor(1.f / alpha)) + 1),
          cos_sin_(new float2[width * num_samples_]),
          s_(new float2[width]) {
        float const m        = float(width_);
        float const half_m   = 0.5f * m;
        float const sqrt_m   = std::sqrt(m);
        float const i_sqrt_m = 1.f / sqrt_m;
        float const iss      = std::floor(1.f / alpha);
        float const dk       = 1.f / (m * iss);

        float const ucot = 1.f / std::tan(alpha * (math::Pi * 0.5f));
        float const cot  = math::Pi * ucot;
        float const csc  = (2.f * math::Pi) / std::sin(alpha * (math::Pi * 0.5f));

        float2 const sx = sqrtc(float2(1.f, -ucot));
        /*
                                float coordinates = 0.5f;
                                for (uint32_t x = 0, block = 0, len = width_, num_samples =
           num_samples_; x < len;
                                         ++x, ++coordinates, block += num_samples) {
                                        float const u = (coordinates - half_m) * i_sqrt_m;
                                        float const cscu = csc * u;
                                        uint32_t ss = 0;
                                        for (float k = -0.5f; k <= 0.5f; k += dk, ++ss) {
                                                float const v = k * sqrt_m;
                                                float const t = v * (cot * v - cscu);
                                                uint32_t const i = block + ss;
                                                cos_sin_[i][0] = std::cos(t);
                                                cos_sin_[i][1] = std::sin(t);
                                        }

                                        s_[x] = math::mulc(sx, cot * u * u);
                                }
                                */

        pool.run_range(
            [this, half_m, sqrt_m, i_sqrt_m, dk, cot, csc, sx](uint32_t /*id*/, int32_t begin,
                                                               int32_t end) {
                for (int32_t x = begin, num_samples = num_samples_; x < end; ++x) {
                    float const coordinates = static_cast<float>(x) + 0.5f;
                    float const u           = (coordinates - half_m) * i_sqrt_m;
                    float const cscu        = csc * u;
                    uint32_t    ss          = 0;
                    for (float k = -0.5f; k <= 0.5f; k += dk, ++ss) {
                        float const    v = k * sqrt_m;
                        float const    t = v * (cot * v - cscu);
                        uint32_t const i = x * num_samples + ss;
                        cos_sin_[i][0]   = std::cos(t);
                        cos_sin_[i][1]   = std::sin(t);
                    }

                    s_[x] = mulc(sx, cot * u * u);
                }
            },
            0, width_);

        samples_ = new char[pool.num_threads() * sizeof(float2) * num_samples_];
    }

    ~Row() {
        delete[] samples_;

        delete[] s_;
        delete[] cos_sin_;
    }

    uint32_t num_samples() const {
        return num_samples_;
    }

    float2 cos_sin(uint32_t x, uint32_t ss) const {
        uint32_t i = x * num_samples_ + ss;
        return cos_sin_[i];
    }

    float2 s(uint32_t x) const {
        return s_[x];
    }

    template <typename T>
    T* row_samples(uint32_t id) const {
        return &reinterpret_cast<T*>(samples_)[id * num_samples_];
    }

  private:
    uint32_t width_;
    uint32_t num_samples_;
    float2*  cos_sin_;
    float2*  s_;

    char* samples_;
};

template <typename Source, typename T>
static void fdft(image::Float2& destination, const Source& source, const Row& row, float alpha,
                 uint32_t id, int32_t begin, int32_t end) {
    int32_t const d = destination.description().dimensions[0];
    int32_t const b = d - 1;

    float const m      = float(d);
    float const sqrt_m = std::sqrt(m);
    float const iss    = std::floor(1.f / alpha);  // 8.f;
    float const norm   = 1.f / (iss * sqrt_m);
    float const dk     = 1.f / (m * iss);

    T* row_samples = row.row_samples<T>(id);

    for (int32_t y = begin; y < end; ++y) {
        uint32_t ss = 0;
        for (float k = -0.5f; k <= 0.5f; k += dk, ++ss) {
            float const tc  = (k + 0.5f) * m;
            row_samples[ss] = sample<T>(source.data(), d, b, tc, y);
        }

        float coordinates = 0.5f;
        for (int32_t x = 0; x < d; ++x, ++coordinates) {
            float2 integration(0.f);

            for (uint32_t i = 0, len = row.num_samples(); i < len; ++i) {
                const T g = row_samples[i];

                integration += mulc_cos_sin(g, row.cos_sin(x, i));
            }

            //		float const  u = (coordinates - half_m) * i_sqrt_m;
            //		float2 const s = math::mulc(sx, cot * u * u);

            float2 const s = row.s(x);

            destination.store(x, y, norm * mulc(s, integration));
        }
    }
}

template <typename Source, typename T>
static void fdft(image::Float2& destination, const Source& source, const Row& row, float alpha,
                 thread::Pool& pool) {
    auto const d = destination.description().dimensions;
    pool.run_range(
        [&destination, &source, &row, alpha](uint32_t id, int32_t begin, int32_t end) {
            fdft<Source, T>(destination, source, row, alpha, id, begin, end);
        },
        0, d[1]);
}

}  // namespace starburst
}  // namespace procedural
