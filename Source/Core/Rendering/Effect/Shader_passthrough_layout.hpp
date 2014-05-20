#pragma once

#include "Rendering/Shader.hpp"
#include <string>
#include <vector>

namespace rendering
{

class Shader_data_layout;
struct Shader_data_struct;

class Shader_passthrough_layout
{

public:

	Shader_passthrough_layout(const Shader_data_layout& layout);

	void set_input_name(const std::string& input_name);
	void set_output_name(const std::string& output_name);
	void set_input_primitive_topology(const std::string& primitive_topology);
	void set_output_primitive_topology(const std::string& primitive_topology);
	void set_max_vertex_count(uint32_t max_vertex_count);

	std::string generate_input_layout_code(Shader::Type type);
	std::string generate_output_layout_code(Shader::Type type);

private:

	std::string generate_vertex_input_layout_code(const Shader_data_struct& data_struct) const;

	std::string generate_passthrough_layout_code(const Shader_data_struct& data_struct, Shader::Type type, bool in);

	std::string generate_pixel_output_layout_code(const Shader_data_struct& data_struct) const;

	static std::string get_glsl_primitive_topology(const std::string& primitive_topology);

	std::string m_input_name;
	std::string m_output_name;
	std::string m_input_primitive_topology;
	std::string m_output_primitive_topology;
	uint32_t m_max_vertex_count;

	uint32_t next__passthrough_index;

	const Shader_data_layout& m_layout;
};

}


