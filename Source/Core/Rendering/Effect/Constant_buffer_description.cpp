#include "Constant_buffer_description.hpp"
#include "File/File_stream.hpp"
#include "Parsing/Json.hpp"
#include <sstream>
#include <iostream>

namespace rendering
{

bool Constant_buffer_description::load(Constant_buffer_description& description, const rapidjson::Value& value)
{
	if (!Shader_data_struct::load(description, value))
	{
		return false;
	}

	description.circular_size = json::read_uint(value, "circular_size", 1);

	return true;
}

std::string Constant_buffer_description_container::generate_constant_buffers_code() const
{
	if (descriptions.empty())
	{
		return "";
	}

	uint32_t binding_point = 0;

	std::ostringstream code;

	for (auto& description : descriptions)
	{
		if (description.elements.empty())
		{
			continue;
		}

		code << "layout(std140, binding = " << binding_point << ") uniform " << description.name << std::endl << "{" << std::endl;

		for (auto& element : description.elements)
		{
			code << "\t" << Shader_data_type::glsl_mapping(element.type) << " " << element.name;

			if (element.array_size)
			{
				code << "[" << element.array_size << "]";
			}

			code << ";" << std::endl;
		}

		code << "};" << std::endl;

		++binding_point;
	}

	return code.str();
}

void Constant_buffer_description_container::load(Constant_buffer_description_container& container, const rapidjson::Value& container_value)
{
	if (!container_value.IsArray())
	{
		return;
	}

	for (rapidjson::SizeType i = 0; i < container_value.Size(); ++i)
	{
		Constant_buffer_description buffer_description;

		if (Constant_buffer_description::load(buffer_description, container_value[i]))
		{
			container.descriptions.push_back(buffer_description);
		}
	}
}

bool Constant_buffer_description_cache::load(file::Input_stream& stream)
{
	rapidjson::Document root;

	if (!json::parse(root, stream))
	{
		return false;
	}

	for (auto n = root.MemberBegin(); n != root.MemberEnd(); ++n)
	{
		const std::string node_name = n->name.GetString();
		const rapidjson::Value& node_value = n->value;

		if ("buffers" == node_name)
		{
			if (node_value.IsArray())
			{
				for (rapidjson::SizeType i = 0; i < node_value.Size(); ++i)
				{
					Constant_buffer_description buffer_description;

					if (Constant_buffer_description::load(buffer_description, node_value[i]))
					{
						descriptions_[buffer_description.name] = buffer_description;
					}
				}
			}
		}
	}

	return true;
}

bool Constant_buffer_description_cache::fill_description_from_class(Constant_buffer_description& description) const
{
	auto d = descriptions_.find(description.name);

	if (descriptions_.end() == d)
	{
		return false;
	}

	description = d->second;

	return true;
}

}
