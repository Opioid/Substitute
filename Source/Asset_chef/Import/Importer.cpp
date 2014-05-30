#include "Importer.hpp"
#include "Model/Model.hpp"
#include "Math/Vector.inl"
#include "Parsing/Json.hpp"
#include <cstdint>
#include <iostream>
#include <fstream>

Model* Importer::read(const std::string& name) const
{
	std::ifstream stream(name);

	if (!stream)
	{
		std::cout << "Could not open \"" << name << "\"." << std::endl;
		return nullptr;
	}

	rapidjson::Document root;

	if (!json::parse(root, stream))
	{
		std::cout << "Could not parse json string: " << json::read_error(root, stream) << std::endl;
		return nullptr;
	}
	
	rapidjson::Value::Member* geometry_node = root.FindMember("geometry");

	if (!geometry_node)
	{
		std::cout << "Could not find model node." << std::endl;
		return nullptr;
	}

	const rapidjson::Value& geometry_value = geometry_node->value;

	Model* model = new Model;

	for (auto n = geometry_value.MemberBegin(); n != geometry_value.MemberEnd(); ++n)
	{
		const std::string node_name = n->name.GetString();
		const rapidjson::Value& node_value = n->value;

		if (node_name == "groups")
		{
			if (!node_value.IsArray())
			{
				std::cout << "groups node is not an array as expected." << std::endl;
				continue;
			}

			for (rapidjson::SizeType i = 0; i < node_value.Size(); ++i)
			{
				const rapidjson::Value& group_value = node_value[i];

				Model::Group group;

				group.material_index = json::read_uint(group_value, "material_index", 0);
				group.start_index    = json::read_uint(group_value, "start_index", 0);
				group.num_indices    = json::read_uint(group_value, "num_indices", 0);

				model->groups.push_back(group);
			}

		}
		else if (node_name == "primitive_topology")
		{
			model->primitive_topology = get_primitve_topology(node_value.GetString());
		}
		else if (node_name == "positions")
		{
			if (!node_value.IsArray())
			{
				std::cout << "positions node is not an array as expected." << std::endl;
				continue;
			}

			for (rapidjson::SizeType i = 0; i < node_value.Size(); ++i)
			{
				model->positions.push_back(json::read_float3(node_value[i]));
			}
		}
		else if (node_name == "texture_coordinates_0")
		{
			if (!node_value.IsArray())
			{
				std::cout << "texture_coordinates0 node is not an array as expected." << std::endl;
				continue;
			}

			for (rapidjson::SizeType i = 0; i < node_value.Size(); ++i)
			{
				model->texture_coordinates.push_back(json::read_float2(node_value[i]));
			}
		}
		else if (node_name == "normals")
		{
			if (!node_value.IsArray())
			{
				std::cout << "normals node is not an array as expected." << std::endl;
				continue;
			}

			for (rapidjson::SizeType i = 0; i < node_value.Size(); ++i)
			{
				model->normals.push_back(json::read_float3(node_value[i]));
			}
		}
		else if (node_name == "tangents_and_bitangent_signs")
		{
			/*
			if (!node_value.IsArray())
			{
				std::cout << "tangents node is not an array as expected." << std::endl;
				continue;
			}

			for (rapidjson::SizeType i = 0; i < node_value.Size(); ++i)
			{
				model->m_tangents_and_bitangent_signs.push_back(json::read_float4(node_value[i]));
			}
			*/
		}		
		else if (node_name == "indices")
		{
			if (!node_value.IsArray())
			{
				std::cout << "bitantents node is not an array as expected." << std::endl;
				continue;
			}

			for (rapidjson::SizeType i = 0; i < node_value.Size(); ++i)
			{
				model->indices.push_back(node_value[i].GetUint());
			}
		}
	}

	return model;
}

rendering::Primitive_topology Importer::get_primitve_topology(const std::string& name)
{
	if ("point_list" == name)
	{
		return rendering::Primitive_topology::Point_list;
	}
	else if ("triangle_list" == name)
	{
		return rendering::Primitive_topology::Triangle_list;
	}
	else
	{
		return rendering::Primitive_topology::Unknown;
	}
}
