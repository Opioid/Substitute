#pragma once

#include "Resources/Resource.hpp"
#include "Resources/Manageable.hpp"
#include "Resources/Reference_counter.hpp"
#include "Resources/Handle.hpp"
#include "Math/Vector.hpp"
#include "Rendering/Color.hpp"
#include "Flags/Flags.hpp"

namespace rendering { class Shader_resource_view; }

namespace scene
{

class Material : public Resource, public Manageable, public Reference_counter
{

public:

	enum class Sampler
	{
		Color,
		Normals,
		Surface0,
		Surface1,
		Enum_count
	};

	enum class Technique
	{
		Color,
		Color_normal_map,
		Color_map,
		Color_map_alpha_transparency,
		Color_map_surface_map,
		Color_map_normal_map,
		Color_map_alpha_transparency_normal_map,
		Color_map_normal_map_parallax,
		Color_map_normal_map_surface_map,
		Color_map_normal_map_surface_map_emissive_map,
		Color_map_normal_map_parallax_surface_map
	};

	enum class Property
	{
		Nothing		 	   = 1 << 0,
		Alpha_transparency = 1 << 1,
		Two_sided		   = 1 << 2,
		Array			   = 1 << 3,
	};

	enum class Particle_property
	{
		Nothing	= 1 << 0,
		Soft    = 1 << 1
	};

	enum class Shading
	{
		Default,
		Transparent,
		Custom
	};

	enum class Blending
	{
		None,
		Alpha,
		One
	};

	typedef Flags<Property, uint32_t> Material_properties;
	typedef Flags<Particle_property, uint32_t> Particle_properties;

	Material(const std::string& name);
	~Material();

	Technique technique() const;

	uint32_t property_mask() const;

	Shading shading() const;
	Blending blending() const;

	bool has_alpha_transparency() const;
	void set_alpha_transparency(bool alpha_transparency);

	bool is_two_sided() const;
	void set_two_sided(bool two_sided);

	bool is_array() const;

	bool is_soft_particle() const;
	void set_soft_particle(bool soft_particle);

	const rendering::Color4& color_and_emissive_factor() const;

	const rendering::Color3& color() const;
	void                     set_color(const rendering::Color3& color);

	float emissive_factor() const;
	void  set_emissive_factor(float emissive_factor);

	const float2& metallic_and_roughness() const;
	void          set_metallic_and_roughness(const float2& metallic_and_roughness);

	float metallic() const;
	void  set_metallic(float metallic);

	float roughness() const;
	void  set_roughness(float roughness);

	const float2& height_scale() const;
	void          set_height_scale(const float2& scale);

	const Handle<rendering::Shader_resource_view>* textures() const;

	bool has_texture(Sampler sampler) const;

	void determine_technique(bool has_height, bool has_emissive);

private:

	Technique technique_;

	Material_properties properties_;
	Particle_properties particle_properties_;

	Shading  shading_;
	Blending blending_;

	rendering::Color4 color_and_emissive_factor_;
	float2            metallic_and_roughness_;
	float2            height_scale_;

	Handle<rendering::Shader_resource_view> textures_[static_cast<size_t>(Sampler::Enum_count)];

	friend class Material_provider;
};

}

