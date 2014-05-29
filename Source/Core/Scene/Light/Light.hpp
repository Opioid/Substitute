#pragma once

#include "../Entity.hpp"
#include "Rendering/Color.hpp"
#include "Resources/Handle.hpp"

namespace rendering
{

class Shader_resource_view;

}

namespace scene
{

class Light : public Entity
{

public:

	enum class Type
	{
		Directional,
		Point,
		Spot
	};

    Light();

    void init(Type type);
    void clear();

	Type type() const;

	const rendering::Color3& color() const;
	void set_color(const rendering::Color3& color);

	float lumen() const;
	void set_lumen(float lumen);

	float falloff_epsilon() const;
	void set_falloff_epsilon(float falloff_epsilon);

	float3 directional_energy() const;
	float4 point_energy_and_range() const;
	float4 spot_energy_and_range() const;

	bool casts_shadow() const;
	void set_casts_shadow(bool shadow);

	float fov() const;
	void  set_fov(float fov);

	const Handle<rendering::Shader_resource_view>& texture() const;
	void set_texture(const Handle<rendering::Shader_resource_view>& texture);

	float4x4 calculate_view_projection() const;
	float4x4 calculate_view_projection(float z_near, float z_far) const;

	static const float4x4& texture_transform();

private:

	float4 energy_and_range(float solid_angle) const;

	static float calculate_fixed_range(float energy, float push_back);

    Type type_;

	rendering::Color3 color_;
	float             lumen_;
	float             falloff_epsilon_;

	bool casts_shadow_;

	Handle<rendering::Shader_resource_view> texture_;

	// for spot lights only
	float fov_;
};

}
