#include "Settings.hpp"
#include "Parsing/Json.hpp"
#include "Parsing/Json_stream.hpp"
#include <fstream>

Settings::Settings() : output_container(Storage_options::Container::DDS)
{}

bool Settings::save(const std::string& file_name) const
{
	std::ofstream stream(file_name);

	if (!stream)
	{
		return false;
	}

	stream << "{" << std::endl;

	stream << "\t\"output_directory\": \"" << output_directory << "\"," << std::endl;

	stream << "\t\"material_folder\": \"" << material_folder << "\"," << std::endl;

	stream << "\t\"texture_folder\": \"" << texture_folder << "\"," << std::endl;

	stream << "\t\"output_container\": \"" << Storage_options::get_container_name(output_container) << "\"" << std::endl;

	stream << "}" << std::endl;

	return true;
}

bool Settings::load(const std::string& file_name)
{
	std::ifstream stream(file_name);

	if (!stream)
	{
		return false;
	}

	rapidjson::Document root;

	if (!json::parse(root, stream))
	{
		return false;
	}

	for (auto n = root.MemberBegin(); n != root.MemberEnd(); ++n)
	{
		const std::string node_name = n->name.GetString();
		const rapidjson::Value& node_value = n->value;

		if ("output_directory" == node_name)
		{
			output_directory = node_value.GetString();
		}
		else if ("material_folder" == node_name)
		{
			material_folder = node_value.GetString();
		}
		else if ("texture_folder" == node_name)
		{
			texture_folder = node_value.GetString();
		}
		else if ("output_container" == node_name)
		{
			output_container = Storage_options::get_container_from_name(node_value.GetString());
		}
	}

	return true;
}
