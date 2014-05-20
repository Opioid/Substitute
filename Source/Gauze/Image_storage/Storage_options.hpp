#pragma once

#include "Rendering/Data_format.hpp"
#include <string>

class Storage_options
{

public:

	enum class Container
	{
		DDS,
		PNG,
		Count
	};

	Storage_options(rendering::Data_format::Value format, bool alpha, bool normal_map, Container container);

	rendering::Data_format::Value get_format() const;

	bool has_alpha() const;

	bool is_normal_map() const;

	Container get_container() const;

	std::string get_file_extension() const;

	static std::string get_file_extension(Container container);

	static std::string get_container_name(Container container);
	static Container get_container_from_name(const std::string& name);

private:

	rendering::Data_format::Value format_;

	bool alpha_;
	bool normal_map_;

	Container container_;
};
