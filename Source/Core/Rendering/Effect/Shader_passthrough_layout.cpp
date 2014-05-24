#include "Shader_passthrough_layout.hpp"
#include "Shader_data_layout.hpp"
#include "Shader_data_type.hpp"
#include <sstream>

namespace rendering
{

Shader_passthrough_layout::Shader_passthrough_layout(const Shader_data_layout& layout) : max_vertex_count_(0), next_passthrough_index_(0), layout_(layout)
{}

void Shader_passthrough_layout::set_input_name(const std::string& input_name)
{
	input_name_ = input_name;
}

void Shader_passthrough_layout::set_output_name(const std::string& output_name)
{
	output_name_ = output_name;
}

void Shader_passthrough_layout::set_input_primitive_topology(const std::string& primitive_topology)
{
	input_primitive_topology_ = primitive_topology;
}

void Shader_passthrough_layout::set_output_primitive_topology(const std::string& primitive_topology)
{
	output_primitive_topology_ = primitive_topology;
}

void Shader_passthrough_layout::set_max_vertex_count(uint32_t max_vertex_count)
{
	max_vertex_count_ = max_vertex_count;
}

std::string Shader_passthrough_layout::generate_input_layout_code(Shader::Type type)
{
	const auto& descriptions = layout_.descriptions();
	auto p = descriptions.find(input_name_);

	if (descriptions.end() == p)
	{
		return "";
	}

	auto& description = p->second;

	switch (type)
	{
	case Shader::Type::Vertex:
		return generate_vertex_input_layout_code(description);
	case Shader::Type::Geometry:
		return generate_passthrough_layout_code(description, type, true);
	case Shader::Type::Pixel:
		return generate_passthrough_layout_code(description, type, true);
	default:
		return "";
	}
}

std::string Shader_passthrough_layout::generate_output_layout_code(Shader::Type type)
{
	const auto& descriptions = layout_.descriptions();
	auto p = descriptions.find(output_name_);

	if (descriptions.end() == p)
	{
		return "";
	}

	auto& description = p->second;

	switch (type)
	{
	case Shader::Type::Vertex:
		return generate_passthrough_layout_code(description, type, false);
	case Shader::Type::Geometry:
		return generate_passthrough_layout_code(description, type, false);
	case Shader::Type::Pixel:
		return generate_pixel_output_layout_code(description);
	default:
		return "";
	}
}

std::string Shader_passthrough_layout::generate_vertex_input_layout_code(const Shader_data_struct& data_struct) const
{
	std::ostringstream code;

	for (size_t i = 0; i < data_struct.elements.size(); ++i)
	{
		code << "layout(location = " << i << ") in " << Shader_data_type::glsl_mapping(data_struct.elements[i].type) << " vertex_in_" << data_struct.elements[i].name << ";" << std::endl;
	}

	code << std::endl;

	return code.str();
}

std::string Shader_passthrough_layout::generate_passthrough_layout_code(const Shader_data_struct& data_struct, Shader::Type type, bool in)
{
	std::ostringstream code;

	if (Shader::Type::Geometry == type)
	{
		if (in)
		{
			code << "layout(" << get_glsl_primitive_topology(input_primitive_topology_) << ") in;" << std::endl;
		}
		else
		{
			code << "layout(" << get_glsl_primitive_topology(output_primitive_topology_) << ", max_vertices = " << max_vertex_count_ << ") out;" << std::endl;
		}
	}

	if (in)
	{
		code << "in Passthrough" << (next_passthrough_index_ - 1);
	}
	else
	{
		code << "out Passthrough" << (next_passthrough_index_++);
	}

	code << std::endl << "{" << std::endl;

	for (size_t i = 0; i < data_struct.elements.size(); ++i)
	{
		code << "\t" << Shader_data_type::glsl_mapping(data_struct.elements[i].type) << " " << data_struct.elements[i].name << ";" << std::endl;
	}

	code << "} ";

	if (in)
	{
		if (Shader::Type::Geometry == type)
		{
			code << "geometry_in[]";
		}
		else if (Shader::Type::Pixel == type)
		{
			code << "pixel_in";
		}
	}
	else
	{
		if (Shader::Type::Vertex == type)
		{
			code << "vertex_out";
		}
		else if (Shader::Type::Geometry == type)
		{
			code << "geometry_out";
		}
	}

	code << ";" << std::endl;

	return code.str();
}

std::string Shader_passthrough_layout::generate_pixel_output_layout_code(const Shader_data_struct& data_struct) const
{
	std::ostringstream code;

	for (size_t i = 0; i < data_struct.elements.size(); ++i)
	{
		code << "layout(location = " << i << ") out " << Shader_data_type::glsl_mapping(data_struct.elements[i].type) << " pixel_out_" << data_struct.elements[i].name << ";" << std::endl;
	}

	code << std::endl;

	return code.str();
}

std::string Shader_passthrough_layout::get_glsl_primitive_topology(const std::string& primitive_topology)
{
	if ("Point_list" == primitive_topology)
	{
		return "points";
	}
	else if ("Triangle_strip" == primitive_topology)
	{
		return "triangle_strip";
	}

	return "";
}

}


