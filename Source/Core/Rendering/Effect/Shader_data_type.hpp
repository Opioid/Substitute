#pragma once

#include "Parsing/Rapidjson_types.hpp"
#include <string>
#include <vector>
#include <cstdint>

namespace rendering
{

struct Shader_data_type
{
	std::string type;
	std::string name;
	std::string semantic;
	uint32_t    array_size = 0;

	static std::string glsl_mapping(const std::string& type);
};

struct Shader_data_struct
{
	std::string name;

	std::vector<Shader_data_type> elements;

	static bool load(Shader_data_struct& data_struct, const rapidjson::Value& struct_value);
};

}

