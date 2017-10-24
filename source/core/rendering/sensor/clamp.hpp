#pragma once

#include "base/math/vector4.hpp"

namespace rendering::sensor::clamp {

class Identity {

public:

	float4 clamp(const float4& color) const;
};

class Clamp {

public:

	Clamp(const float3& max);

	float4 clamp(const float4& color) const;

private:

	float3 max_;
};

}
