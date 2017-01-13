#pragma once

#include "sampler_settings.hpp"
#include "image/texture/texture_types.hpp"
#include "base/json/json_types.hpp"
#include "base/math/vector.hpp"
#include <memory>
#include <vector>

namespace thread { class Pool; }

namespace scene {

struct Renderstate;
class Worker;

namespace entity { struct Composed_transformation; }

namespace shape { class Shape; }

namespace material {

class Sample;

class Sample_cache;

class BSSRDF;

class Material {

public:

	using Transformation = entity::Composed_transformation;
	using Sampler_filter = material::Sampler_settings::Filter;

	Material(Sample_cache& sample_cache, const Sampler_settings& sampler_settings, bool two_sided);

	virtual ~Material();

	void set_mask(const Texture_adapter& mask);

	void set_parameters(const json::Value& parameters);

	virtual void tick(float absolute_time, float time_slice);

	virtual const Sample& sample(float3_p wo, const Renderstate& rs,
								 const Worker& worker, Sampler_filter filter) = 0;

	virtual const BSSRDF& bssrdf(const Worker& worker);

	virtual float3 sample_radiance(float3_p wi, float2 uv, float area, float time,
								   const Worker& worker, Sampler_filter filter) const;

	virtual float3 average_radiance(float area) const;

	virtual bool has_emission_map() const;

	virtual float2 radiance_sample(float2 r2, float& pdf) const;

	virtual float emission_pdf(float2 uv, const Worker& worker, Sampler_filter filter) const;

	virtual float opacity(float2 uv, float time, const Worker& worker,
						  Sampler_filter filter) const;

	virtual void prepare_sampling(const shape::Shape& shape, uint32_t part,
								  const Transformation& transformation,
								  float area, bool importance_sampling,
								  thread::Pool& pool);

	virtual bool is_animated() const;

	virtual bool is_subsurface() const;

	virtual size_t num_bytes() const = 0;

	uint32_t sampler_key() const;

	bool is_masked() const;
	bool is_emissive() const;
	bool is_two_sided() const;

protected:

	virtual void set_parameter(const std::string& name, const json::Value& value);

	Sample_cache& sample_cache_;

	Texture_adapter mask_;

private:

	uint32_t sampler_key_;

	bool two_sided_;
};

using Material_ptr = std::shared_ptr<material::Material>;
using Materials = std::vector<Material_ptr>;

}}
