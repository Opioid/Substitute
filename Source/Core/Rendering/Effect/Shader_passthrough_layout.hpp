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

	std::string input_name_;
	std::string output_name_;
	std::string input_primitive_topology_;
	std::string output_primitive_topology_;
	uint32_t max_vertex_count_;

	uint32_t next_passthrough_index_;

	const Shader_data_layout& layout_;
};

}


