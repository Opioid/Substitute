#include "Light.hpp"
#include "Rendering/Resource_view.hpp"

namespace scene
{

Light::Light() : type_(Type::Directional), color_(rendering::color3::white), lumen_(1.f), falloff_epsilon_(0.01f), casts_shadow_(false), fov_(0.f)
{}

void Light::init(Type type)
{
    type_ = type;
}

void Light::clear()
{
    texture_ = nullptr;
}

Light::Type Light::get_type() const
{
	return type_;
}

const rendering::Color3& Light::color() const
{
	return color_;
}

void Light::set_color(const rendering::Color3& color)
{
	color_ = color;
}

float Light::get_lumen() const
{
	return lumen_;
}

void Light::set_lumen(float lumen)
{
	lumen_ = lumen;
}

float Light::get_falloff_epsilon() const
{
	return falloff_epsilon_;
}

void Light::set_falloff_epsilon(float falloff_epsilon)
{
	falloff_epsilon_ = falloff_epsilon;
}

float3 Light::get_directional_energy() const
{
	return lumen_ * color_;
}

float4 Light::get_point_energy_and_range() const
{
	return get_energy_and_range(4.f * math::pi);
}

float4 Light::get_spot_energy_and_range() const
{
	return get_energy_and_range(2.f * math::pi * (1.f - cos(fov_ * 0.5f)));
}

bool Light::casts_shadow() const
{
	return casts_shadow_;
}

void Light::set_casts_shadow(bool shadow)
{
	casts_shadow_ = shadow;
}

float Light::FOV() const
{
	return fov_;
}

void Light::set_fov(float fov)
{
	fov_ = fov;
}

const Handle<rendering::Shader_resource_view>& Light::texture() const
{
	return texture_;
}

void Light::set_texture(const Handle<rendering::Shader_resource_view>& texture)
{
	texture_ = texture;
}

float4x4 Light::calculate_view_projection() const
{
	float4 energy_and_range = get_spot_energy_and_range();

	return calculate_view_projection(0.25f, energy_and_range.w);
}

float4x4 Light::calculate_view_projection(float z_near, float z_far) const
{
	const float3 position = world_position();

	float4x4 view;
	set_look_at(view, position, position + world_direction(), world_up());

	float4x4 projection;
	set_perspective(projection, fov_, 1.f, z_near, z_far);

	return view * projection;
}

float4 Light::get_energy_and_range(float solid_angle) const
{
	float d = lumen_ / solid_angle;
	float3 energy = d * color_;

	uint32_t i;
	float m = energy.absolute_max(i);

	return float4(energy, calculate_fixed_range(m, falloff_epsilon_));
}

const float4x4& Light::get_texture_transform()
{
	static const float4x4 texture_transform(0.5f, 0.f,  0.f, 0.f,
											0.f,  0.5f, 0.f, 0.f,
											0.f,  0.f,  1.f, 0.f,
											0.5f, 0.5f, 0.f, 1.f);

	return texture_transform;
}

float Light::calculate_fixed_range(float energy, float push_back)
{
	return sqrt(energy / push_back);
}

}
