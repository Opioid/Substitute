#include "Shader_data_layout.hpp"
#include "Shader_data_type.hpp"
#include <sstream>

namespace rendering
{

const Shader_data_layout::Data_structs& Shader_data_layout::descriptions() const
{
	return data_structs_;
}

void Shader_data_layout::add_description(const std::string& name, const Shader_data_struct& data_struct)
{
	if (!data_struct.elements.empty())
	{
		data_structs_[name] = data_struct;
	}
}

std::string Shader_data_layout::generate_constant_buffer_code() const
{
	if (data_structs_.empty())
	{
		return "";
	}

	std::ostringstream code;

	for (auto b = data_structs_.begin(); b != data_structs_.end(); ++b)
	{
		auto& constants = b->second;

		if (constants.elements.empty())
		{
			continue;
		}

		code << "layout(std140) uniform " << b->first << std::endl << "{" << std::endl;

		for (size_t i = 0; i < constants.elements.size(); ++i)
		{
			code << "\t" << Shader_data_type::glsl_mapping(constants.elements[i].type) << " " << constants.elements[i].name;

			if (constants.elements[i].array_size)
			{
				code << "[" << constants.elements[i].array_size << "]";
			}

			code << ";" << std::endl;
		}

		code << "};" << std::endl;
	}

	return code.str();
}

}
