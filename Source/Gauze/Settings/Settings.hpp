#pragma once

#include "Image_storage/Storage_options.hpp"
#include <string>

struct Settings
{
	Settings();

	std::string output_directory;

	std::string material_folder;

	std::string texture_folder;

	Storage_options::Container output_container;

	bool save(const std::string& file_name) const;

	bool load(const std::string& file_name);
};
