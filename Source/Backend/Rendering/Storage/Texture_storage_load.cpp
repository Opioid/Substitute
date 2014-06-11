#include "Texture_storage_load.hpp"
#include "Texture_storage_load_DDS.hpp"
#include "Texture_storage_load_SUI.hpp"
#include "Rendering/Texture_data_adapter.hpp"
#include "Texture_data_adapter_cube.hpp"
#include "File/File_type.hpp"
#include "String/String.hpp"
#include "Parsing/Json.hpp"

namespace rendering
{

std::shared_ptr<Texture_data_adapter> Texture_storage::load_texture(file::Input_stream& stream, bool treat_as_linear, std::string& error_message)
{
	file::File_type file_type = query_file_type(stream);

	if (file::File_type::DDS == file_type)
	{
		return texture_storage::load_DDS_texture(stream, treat_as_linear, error_message);
	}
	else if (file::File_type::SUI == file_type)
	{
		return texture_storage::load_SUI_texture(stream, error_message);
	}

	return nullptr;
}

std::shared_ptr<Texture_data_adapter> Texture_storage::load_texture_cube(file::Input_stream& stream, bool treat_as_linear, std::string& error_message)
{
	file::File_type file_type = query_file_type(stream);

	if (file::File_type::DDS == file_type)
	{
		return texture_storage::load_DDS_texture(stream, treat_as_linear, error_message);
	}
	else if (file::File_type::SUI == file_type)
	{
		return texture_storage::load_SUI_texture(stream, error_message);
	}

	rapidjson::Document root;
	if (!json::parse(root, stream))
	{
		error_message = "Texture_storage::load_texture_cube(): \"" + stream.name() + "\" could not be loaded: failed to parse the file: " + json::read_error(root, stream);

		return nullptr;
	}

	std::shared_ptr<Texture_data_adapter> texture_data[6];

	for (auto n = root.MemberBegin(); n != root.MemberEnd(); ++n)
	{
		const std::string node_name = n->name.GetString();
		const rapidjson::Value& node_value = n->value;

		if (node_name == "type")
		{
		}
		else if (node_name == "encoding")
		{
		}
		else if (node_name == "sources")
		{
			if (!node_value.IsArray())
			{
				return nullptr;
			}

			for (rapidjson::SizeType i = 0; i < node_value.Size() && i < 6; ++i)
			{
				std::string name_2D = node_value[i].GetString();

				file::Input_stream stream_2D(name_2D);

				if (!stream_2D)
				{
					error_message = "Texture_storage::load_texture_cube(): \"" + name_2D + "\" could not be loaded.";

					return nullptr;
				}

				texture_data[i] = load_texture(stream_2D, treat_as_linear, error_message);

				if (!texture_data[i])
				{
					return nullptr;
				}
			}
		}
	}

	Texture_description description = texture_data[0]->description();

	return std::make_shared<Texture_data_adapter_cube>(description, texture_data);
}

}
