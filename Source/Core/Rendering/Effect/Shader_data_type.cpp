#include "Shader_data_type.hpp"
#include "Parsing/Json.hpp"
#include <map>

namespace rendering
{

std::string Shader_data_type::get_glsl_mapping(const std::string& type)
{
	static std::map<std::string, std::string> mapping;

	if (mapping.empty())
	{
		mapping["float"] = "float";

		mapping["float2"] = "vec2";
		mapping["float3"] = "vec3";
		mapping["float4"] = "vec4";

		mapping["float3x3"] = "mat3";
		mapping["float4x4"] = "mat4";
	}

	auto t = mapping.find(type);

	std::string result = "unknown";

	if (mapping.end() != t)
	{
		result = t->second;
	}

	return result;
}

bool Shader_data_struct::load(Shader_data_struct& data_struct, const rapidjson::Value& struct_value)
{
	const rapidjson::Value::Member* name_node = struct_value.FindMember("name");

	if (!name_node)
	{
		return false;
	}

	data_struct.name = name_node->value.GetString();

	const rapidjson::Value::Member* data_node = struct_value.FindMember("data");

	if (!data_node)
	{
		return true;
	}

	const rapidjson::Value& data_value = data_node->value;

	if (!data_value.IsArray())
	{
		return true;
	}

	for (rapidjson::SizeType i = 0; i < data_value.Size(); ++i)
	{
		Shader_data_type element;

		const rapidjson::Value& e = data_value[i];

		for (auto n = e.MemberBegin(); n < e.MemberEnd(); ++n)
		{
			const std::string node_name = n->name.GetString();
			const rapidjson::Value& node_value = n->value;

			if ("type" == node_name)
			{
				element.type = node_value.GetString();
			}
			else if ("name" == node_name)
			{
				element.name = node_value.GetString();
			}
			else if ("semantic" == node_name)
			{
				element.semantic = node_value.GetString();
			}
			else if ("array_size" == node_name)
			{
				element.array_size = node_value.GetUint();
			}
		}

		data_struct.elements.push_back(element);
	}

	return true;
}

}
