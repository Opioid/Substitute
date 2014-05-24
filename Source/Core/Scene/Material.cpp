#include "Material.hpp"
#include "Rendering/Resource_view.hpp"
#include "Math/Math.hpp"

namespace scene
{

Material::Material(const std::string& name) :
	Manageable(name), technique_(Technique::Color),
	color_and_emissive_factor_(0.5f, 0.5f, 0.5f, 0.f),
	metallic_and_roughness_(0.f, 0.7f),
	height_scale_(0.02f, -0.02f)
{
	for (size_t i = 0; i < static_cast<size_t>(Sampler::Enum_count); ++i)
	{
		textures_[i] = nullptr;
	}
}

Material::~Material()
{}

Material::Technique Material::technique() const
{
	return technique_;
}

uint32_t Material::get_property_mask() const
{
	return static_cast<uint32_t>(properties_);
}

bool Material::has_alpha_transparency() const
{
	return properties_.is_set(Property::Alpha_transparency);
}

void Material::set_alpha_transparency(bool alpha_transparency)
{
	properties_.set(Property::Alpha_transparency, alpha_transparency);
}

bool Material::is_two_sided() const
{
	return properties_.is_set(Property::Two_sided);
}

void Material::set_two_sided(bool two_sided)
{
	properties_.set(Property::Two_sided, two_sided);
}

const rendering::Color4& Material::color_and_emissive_factor() const
{
	return color_and_emissive_factor_;
}

const rendering::Color3& Material::color() const
{
	return color_and_emissive_factor_.rgb;
}

void Material::set_color(const rendering::Color3& color)
{
	color_and_emissive_factor_.rgb = color;
}

float Material::emissive_factor() const
{
	return color_and_emissive_factor_.w * 100.f;
}

void Material::set_emissive_factor(float emissive_factor)
{
	color_and_emissive_factor_.w = emissive_factor / 100.f;
}

const float2& Material::metallic_and_roughness() const
{
	return metallic_and_roughness_;
}

void Material::set_metallic_and_roughness(const float2& metallic_and_roughness)
{
	metallic_and_roughness_ = metallic_and_roughness;
}

float Material::metallic() const
{
	return metallic_and_roughness_.x;
}

void Material::set_metallic(float metallic)
{
	metallic_and_roughness_.x = metallic;
}

float Material::roughness() const
{
	return metallic_and_roughness_.y;
}

void Material::set_roughness(float roughness)
{
	metallic_and_roughness_.y = std::max(roughness, 2.f / 255.f);
}

const float2& Material::height_scale() const
{
	return height_scale_;
}

void Material::set_height_scale(const float2& scale)
{
	height_scale_ = scale;
}

const Handle<rendering::Shader_resource_view>* Material::textures() const
{
	return textures_;
}

bool Material::has_texture(Sampler sampler) const
{
	return textures_[static_cast<size_t>(sampler)] != nullptr;
}

void Material::determine_technique(bool has_height, bool has_emissive)
{
	if (has_texture(Sampler::Color))
	{
		if (has_texture(Sampler::Normals))
		{
			if (has_height)
			{
				if (has_texture(Sampler::Surface0))
				{
					technique_ = Technique::Color_map_normal_map_parallax_surface_map;
				}
				else
				{
					technique_ = Technique::Color_map_normal_map_parallax;
				}
			}
			else
			{
				if (has_texture(Sampler::Surface0))
				{
					technique_ = has_emissive ? Technique::Color_map_normal_map_surface_map_emissive_map : Technique::Color_map_normal_map_surface_map;
				}
				else
				{
					technique_ = has_alpha_transparency() ? Technique::Color_map_alpha_transparency_normal_map : Technique::Color_map_normal_map;
				}
			}
		}
		else
		{
			if (has_texture(Sampler::Surface0))
			{
				technique_ = Technique::Color_map_surface_map;
			}
			else
			{
				technique_ = has_alpha_transparency() ? Technique::Color_map_alpha_transparency : Technique::Color_map;
			}
		}
	}
	else
	{
		if (has_texture(Sampler::Normals))
		{
			technique_ = Technique::Color_normal_map;
		}
		else
		{
			technique_ = Technique::Color;
		}
	}
}

}
