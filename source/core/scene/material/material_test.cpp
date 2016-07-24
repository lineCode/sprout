#include "material_test.hpp"
#include "bxdf.hpp"
#include "base/math/vector.inl"
#include "base/math/print.hpp"
#include <iostream>

namespace scene { namespace material { namespace testing {

bool check(float3_p result, float3_p wi, float3_p wo, float pdf, const Sample::Layer& layer) {
	if (!std::isfinite(pdf)
	||	!math::contains_only_finite(wi)
	||  !math::contains_only_finite(result)) {
		std::cout << "wi: " << wi << std::endl;
		std::cout << "wo: " << wo << std::endl;
		std::cout << "pdf: " << pdf << std::endl;
		std::cout << "reflection " << result << std::endl;
		std::cout << "t " << layer.t << std::endl;
		std::cout << "b " << layer.t << std::endl;
		std::cout << "n " << layer.t << std::endl;
		return false;
	}

	return true;
}

bool check(const bxdf::Result& result, float3_p wo, const Sample::Layer& layer) {
	return check(result.reflection, result.wi, wo, result.pdf, layer);
}

}}}
