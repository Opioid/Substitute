#pragma once

#include <map>
#include <vector>
#include <string>

namespace rendering
{

struct Shader_data_struct;

class Shader_data_layout
{

public:

	typedef std::map<std::string, Shader_data_struct> Data_structs;

	const Data_structs& descriptions() const;

	void add_description(const std::string& name, const Shader_data_struct& data_struct);

	std::string generate_constant_buffer_code() const;

private:

	Data_structs data_structs_;
};

}


