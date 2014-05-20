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
		Two_sided		   = 1 << 2
	};

	typedef tFlags<Property, uint32_t> Material_properties;

	Material(const std::string& name);
	~Material();

	Technique get_technique() const;

	uint32_t get_property_mask() const;

	bool has_alpha_transparency() const;
	void set_alpha_transparency(bool alpha_transparency);

	bool is_two_sided() const;
	void set_two_sided(bool two_sided);

	const rendering::Color4& get_color_and_emissive_factor() const;

	const rendering::Color3& get_color() const;
	void                     set_color(const rendering::Color3& color);

	float get_emissive_factor() const;
	void  set_emissive_factor(float emissive_factor);

	const float2& get_metallic_and_roughness() const;
	void          set_metallic_and_roughness(const float2& metallic_and_roughness);

	float get_metallic() const;
	void  set_metallic(float metallic);

	float get_roughness() const;
	void  set_roughness(float roughness);

	const float2& get_height_scale() const;
	void          set_height_scale(const float2& scale);

	const Handle<rendering::Shader_resource_view>* get_textures() const;

	bool has_texture(Sampler sampler) const;

	void determine_technique(bool has_height, bool has_emissive);

private:

	Technique technique_;

	rendering::Color4 color_and_emissive_factor_;
	float2            metallic_and_roughness_;
	float2            height_scale_;

	Handle<rendering::Shader_resource_view> textures_[static_cast<size_t>(Sampler::Enum_count)];

	Material_properties properties_;

	friend class Material_provider;
};

}

