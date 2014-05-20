#pragma once

#include "Math/Matrix.hpp"
#include "Resources/Handle.hpp"

namespace rendering
{

class Shader_resource_view;

}

namespace scene
{

class Light_probe
{

public:

	enum class Type
	{
		Surrounding,
		Environment
	};

	Light_probe();

    void init(const float3& position, const float3& scale, const Quaternion& rotation);
    void clear();

	const float3& get_world_position() const;

	const float4x4& get_world_transformation() const;

	const Handle<rendering::Shader_resource_view>& get_texture() const;
	void set_texture(const Handle<rendering::Shader_resource_view>& texture);

private:

	float4x4 world_transformation_;

	Handle<rendering::Shader_resource_view> texture_;

public:

	static const Handle<rendering::Shader_resource_view>& get_integrated_brdf();
	static void set_intergrated_brdf(const Handle<rendering::Shader_resource_view>& texture);

private:

	static Handle<rendering::Shader_resource_view> integrated_brdf_;
};

}


