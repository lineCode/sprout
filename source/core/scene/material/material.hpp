#pragma once

#include "material_sample_cache.hpp"
#include "image/texture/texture_2d.hpp"
#include "base/math/vector.hpp"
#include <vector>

namespace image { namespace texture { namespace sampler {

class Sampler_2D;

}}}

namespace scene {

namespace shape {

struct Differential;

}

namespace material {

class Sample;

class IMaterial {
public:

	IMaterial(std::shared_ptr<image::texture::Texture_2D> mask);

	virtual const Sample& sample(const shape::Differential& dg, const math::float3& wo,
								 const image::texture::sampler::Sampler_2D& sampler, uint32_t worker_id) = 0;

	virtual math::float3 sample_emission(math::float2 uv,
										 const image::texture::sampler::Sampler_2D& sampler) const = 0;

	virtual math::float3 average_emission() const = 0;

	virtual const image::texture::Texture_2D* emission_map() const = 0;

	virtual math::float2 emission_importance_sample(math::float2 r2, float& pdf) const;

	virtual float emission_pdf(math::float2 uv, const image::texture::sampler::Sampler_2D& sampler) const;

	virtual void prepare_sampling(bool spherical);

	bool is_masked() const;

	float opacity(math::float2 uv, const image::texture::sampler::Sampler_2D& sampler) const;

protected:

	std::shared_ptr<image::texture::Texture_2D> mask_;
};

template<typename T>
class Material : public IMaterial {
public:

	Material(Sample_cache<T>& cache, std::shared_ptr<image::texture::Texture_2D> mask) :
		IMaterial(mask), cache_(cache) {}

protected:

	Sample_cache<T>& cache_;
};

typedef std::vector<std::shared_ptr<material::IMaterial>> Materials;

}}
