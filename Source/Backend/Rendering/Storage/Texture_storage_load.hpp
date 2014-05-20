#pragma once

#include "File/File_stream.hpp"
#include <memory>

namespace rendering
{

class Texture_data_adapter;

class Texture_storage
{

public:

	static std::shared_ptr<Texture_data_adapter> load_texture_2D(file::Input_stream& stream, bool treat_as_linear, std::string& error_message);

	static std::shared_ptr<Texture_data_adapter> load_texture_3D(file::Input_stream& stream, bool treat_as_linear, std::string& error_message);

	static std::shared_ptr<Texture_data_adapter> load_texture_cube(file::Input_stream& stream, bool treat_as_linear, std::string& error_message);

};

}


