#ifndef SU_CORE_SCENE_MATERIAL_FRESNEL_HPP
#define SU_CORE_SCENE_MATERIAL_FRESNEL_HPP

#include "base/math/vector3.hpp"

namespace scene::material::fresnel {

class Schlick1 {
  public:
    Schlick1(float f0) noexcept;

    float operator()(float wo_dot_h) const noexcept;

  private:
    float const f0_;
};

class Schlick {
  public:
    Schlick(float f0) noexcept;
    Schlick(float3 const& f0) noexcept;

    float3 operator()(float wo_dot_h) const noexcept;

  private:
    float3 const f0_;
};

class Thinfilm {
  public:
    Thinfilm(float external_ior, float thinfilm_ior, float internal_ior, float thickness) noexcept;

    float3 operator()(float wo_dot_h) const noexcept;

  private:
    float const external_ior_;
    float const thinfilm_ior_;
    float const internal_ior_;
    float const thickness_;
};

class Dielectric {
  public:
    Dielectric(float eta_i, float eta_t) noexcept;

    float3 operator()(float wo_dot_h) const noexcept;

  private:
    float const eta_i_;
    float const eta_t_;
};

class Conductor {
  public:
    Conductor(float3 const& eta, float3 const& k) noexcept;

    float3 operator()(float wo_dot_h) const noexcept;

  private:
    float3 const eta_;
    float3 const k_;
};

class Constant {
  public:
    Constant(float f) noexcept;
    Constant(float3 const& f) noexcept;

    float3 operator()(float wo_dot_h) const noexcept;

  private:
    float3 const f_;
};

}  // namespace scene::material::fresnel

#endif
