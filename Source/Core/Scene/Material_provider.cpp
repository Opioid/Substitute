#include "Material_provider.hpp"
#include "Resources/Resource_manager.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Texture_provider.hpp"
#include "Math/Math.hpp"
#include "Math/Vector.inl"
#include "Parsing/Json.hpp"
#include "Logging/Logging.hpp"

namespace scene
{

Material_provider::Material_provider() : Resource_provider("Material")
{}
 
Handle<Material> Material_provider::load(file::Input_stream& stream, Resource_manager& resource_manager, const uint32_t /*flags*/) const
{
	rapidjson::Document root;

	if (!json::parse(root, stream))
	{
		logging::error("Material_provider::load(): \"" + stream.name() + "\" could not be loaded: failed to parse the file: " + json::read_error(root, stream));

		return nullptr;
	}
	
	const rapidjson::Value::Member* rendering = root.FindMember("rendering");

	if (!rendering)
	{
		logging::error("Material_provider::load(): \"" + stream.name() + "\" could not be loaded: No rendering properties found.");

		return nullptr;
	}

	Handle<Material> material(new Material(stream.name()));

	bool has_height = false;
	bool has_emissive = false;

	for (auto n = rendering->value.MemberBegin(); n != rendering->value.MemberEnd(); ++n)
	{
		const std::string node_name = n->name.GetString();
		const rapidjson::Value& node_value = n->value;

	/*	if (n.key() == "technique")
		{
			std::string const technique_name = node.asString();

			if (technique_name == "water")
			{
				technique = Material::Technique::Water;
			}
	*	}
		else*/ if (node_name == "textures")
		{
			if (!node_value.IsArray())
			{
				continue;
			}

			for (rapidjson::SizeType i = 0; i < node_value.Size(); ++i)
			{
				const rapidjson::Value& texture_value = node_value[i];

				std::string file_name = read_file_name(texture_value);

				if (file_name.empty())
				{
					continue;
				}

				Material::Sampler sampler = read_sampler(texture_value);

				Flags<rendering::Texture_provider::Options> flags;

				if (Material::Sampler::Normals  == sampler
				||  Material::Sampler::Surface0 == sampler
				||  Material::Sampler::Surface1 == sampler)
				{
					flags.set(rendering::Texture_provider::Options::Treat_as_linear, false);
				}

				if (Material::Sampler::Color == sampler)
				{
					material->set_alpha_transparency(json::read_bool(texture_value, "alpha_transparency", false));
				}
				else if (Material::Sampler::Surface1 == sampler)
				{
					const rapidjson::Value::Member* range_node = texture_value.FindMember("height_range");

					if (range_node)
					{
						float2 range = json::read_float2(range_node->value);
						material->set_height_scale(float2(range.y - range.x, range.x));

						has_height = true;
					}

					has_emissive = json::read_bool(texture_value, "emissive", false);
				}

				material->textures_[static_cast<size_t>(sampler)] = resource_manager.load<rendering::Shader_resource_view>(file_name, flags.data());
			}
		}
		else if (node_name == "color")
		{
			material->set_color(read_color(node_value));
		}
		else if (node_name == "metallic")
		{
			material->set_metallic(static_cast<float>(node_value.GetDouble()));
		}
		else if (node_name == "roughness")
		{
			material->set_roughness(static_cast<float>(node_value.GetDouble()));
		}
		else if (node_name == "translucency")
		{
		//	node >> material->m_translucency;
		}
		else if (node_name == "emissive_factor")
		{
			material->set_emissive_factor(static_cast<float>(node_value.GetDouble()));
		}
		else if (node_name == "two_sided")
		{
			material->set_two_sided(node_value.GetBool());
		}
	}

	material->determine_technique(has_height, has_emissive);

	return material;
}

Handle<Material> Material_provider::clone(const Handle<Material>& material, Resource_manager& /*resource_manager*/) const
{
	return Handle<Material>(new Material(*material));
}

Material::Sampler Material_provider::read_sampler(const rapidjson::Value& value)
{
	const rapidjson::Value::Member* usage = value.FindMember("usage");

	if (!usage)
	{
		return Material::Sampler::Color;
	}

	const std::string u = usage->value.GetString();

	if (u == "color")
	{
		return Material::Sampler::Color;
	}
	else if (u == "normals") 
	{
		return Material::Sampler::Normals;
	}
	else if (u == "surface0")
	{
		return Material::Sampler::Surface0;
	}
	else if (u == "surface1")
	{
		return Material::Sampler::Surface1;
	}
	else
	{
		return Material::Sampler::Color;
	}
}

rendering::Color3 Material_provider::read_color(const rapidjson::Value& color_value)
{
	rendering::Color3 color;

	if (color_value.IsArray())
	{
		 color = json::read_float3(color_value);
	}
	else
	{
		std::string color_space;

		for (auto n = color_value.MemberBegin(); n != color_value.MemberEnd(); ++n)
		{
			const std::string node_name = n->name.GetString();
			const rapidjson::Value& node_value = n->value;

			if (node_name == "value")
			{
				color = json::read_float3(node_value);
			}
			else if (node_name == "space")
			{
				color_space = node_value.GetString();
			}
		}

		if ("sRGB" == color_space)
		{
			color = rendering::sRGB_to_linear(color);
		}
	}

	return color;
}

std::string Material_provider::read_file_name(const rapidjson::Value& value)
{
	const rapidjson::Value& file_name = value["file"];

	return file_name.GetString();
}

}
