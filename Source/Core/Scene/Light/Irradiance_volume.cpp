#include "Irradiance_volume.hpp"
#include "Rendering/Resource_view.hpp"
#include "Math/Matrix.inl"
#include "Math/Quaternion.inl"

namespace scene
{

Irradiance_volume::Irradiance_volume() : resolution_(uint3::identity), world_transformation_(float4x4::identity)
{}

void Irradiance_volume::init(const uint3& resolution, const float3& position, const float3& scale, const quaternion& rotation)
{
	resolution_ = resolution;

	world_transformation_ = float4x4::identity;

	set_basis(world_transformation_, float3x3(rotation));
	::scale(world_transformation_, scale);
	set_origin(world_transformation_, position);
}

void Irradiance_volume::adjust(const float3& position, const float3& scale)
{
	world_transformation_ = float4x4::identity;

	::scale(world_transformation_, scale);
	set_origin(world_transformation_, position);
}

void Irradiance_volume::clear()
{
	for (uint32_t i = 0; i < num_textures(); ++i)
	{
		textures_[i] = nullptr;
	}
}

bool Irradiance_volume::is_faulty() const
{
	return 0.f == world_transformation_.m00;
}

const uint3& Irradiance_volume::get_resolution() const
{
	return resolution_;
}

uint32_t Irradiance_volume::get_num_probes() const
{
	return resolution_.x * resolution_.y * resolution_.z;
}

const float3& Irradiance_volume::world_position() const
{
	return world_transformation_.rows[3].xyz;
}

const float4x4& Irradiance_volume::world_transformation() const
{
	return world_transformation_;
}

float3 Irradiance_volume::probe_position(uint32_t index) const
{
	uint3 coordinates;

	coordinates.z = index / (resolution_.x * resolution_.y);

	uint32_t temp = index - coordinates.z * resolution_.x * resolution_.y;

	coordinates.y = temp / resolution_.x;

	coordinates.x = temp - coordinates.y * resolution_.x;

	float3 origin = float3(-1.f, -1.f, -1.f);
	float3 end    = float3( 1.f,  1.f,  1.f);

	float3 d = end - origin;

	float3 advance = float3(d.x / float(resolution_.x), d.y / float(resolution_.y), d.z / float(resolution_.z));

	float3 offset = 0.5f * advance + float3(float(coordinates.x) * advance.x, float(coordinates.y) * advance.y, float(coordinates.z) * advance.z);

	return (origin + offset) * world_transformation_;
}

const Handle<rendering::Shader_resource_view>* Irradiance_volume::textures() const
{
	return textures_;
}

void Irradiance_volume::set_texture(const Handle<rendering::Shader_resource_view>& texture, uint32_t index)
{
	textures_[index] = texture;
}

uint32_t Irradiance_volume::num_textures()
{
	return 5;
}

}
