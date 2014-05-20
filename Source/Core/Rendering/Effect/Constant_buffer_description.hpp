#pragma once

#include "Shader_data_type.hpp"
#include <map>

namespace file
{

class Input_stream;

}

namespace rendering
{

struct Constant_buffer_description : public Shader_data_struct
{
	uint32_t circular_size;

	static bool load(Constant_buffer_description& description, const rapidjson::Value& struct_value);
};

struct Constant_buffer_description_container
{
	std::vector<Constant_buffer_description> descriptions;

	std::string generate_constant_buffers_code() const;

	static void load(Constant_buffer_description_container& container, const rapidjson::Value& container_value);
};

class Constant_buffer_description_cache
{

public:

	bool load(file::Input_stream& stream);

	bool fill_description_from_class(Constant_buffer_description& description) const;

private:

	std::map<std::string, Constant_buffer_description> descriptions_;
};

}
