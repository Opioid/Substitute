#pragma once

#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"
#include "Math/Quaternion.hpp"
#include "Resources/Handle.hpp"

namespace rendering
{

class Shader_resource_view;

}

namespace scene
{

class Irradiance_volume
{

public:

	Irradiance_volume();

	void init(const uint3& resolution, const float3& position, const float3& scale, const quaternion& rotation);
	void adjust(const float3& position, const float3& scale);
	void clear();

	bool is_faulty() const;

	const uint3& get_resolution() const;

	uint32_t get_num_probes() const;

	const float3& world_position() const;

	const float4x4& world_transformation() const;

	float3 probe_position(uint32_t index) const;

	const Handle<rendering::Shader_resource_view>* textures() const;
	void set_texture(const Handle<rendering::Shader_resource_view>& texture, uint32_t index);

	static uint32_t num_textures();

private:

	uint3 resolution_;

	float4x4 world_transformation_;

	Handle<rendering::Shader_resource_view> textures_[5];
};

}


