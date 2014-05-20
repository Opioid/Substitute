#include "Light_probe.hpp"
#include "Rendering/Resource_view.hpp"

namespace scene
{

Light_probe::Light_probe() : world_transformation_(float4x4::identity)
{}

void Light_probe::init(const float3& position, const float3& scale, const Quaternion& rotation)
{
    world_transformation_ = float4x4::identity;

	set_basis(world_transformation_, float3x3(rotation));
	::scale(world_transformation_, scale);
	set_origin(world_transformation_, position);
}

void Light_probe::clear()
{
    texture_ = nullptr;
}

const float3& Light_probe::get_world_position() const
{
	return world_transformation_.rows[3].xyz;
}

const float4x4& Light_probe::get_world_transformation() const
{
	return world_transformation_;
}

const Handle<rendering::Shader_resource_view>& Light_probe::get_texture() const
{
	return texture_;
}

void Light_probe::set_texture(const Handle<rendering::Shader_resource_view>& texture)
{
	texture_ = texture;
}

const Handle<rendering::Shader_resource_view>& Light_probe::get_integrated_brdf()
{
	return integrated_brdf_;
}

void Light_probe::set_intergrated_brdf(const Handle<rendering::Shader_resource_view>& texture)
{
	integrated_brdf_ = texture;
}

Handle<rendering::Shader_resource_view> Light_probe::integrated_brdf_;

}