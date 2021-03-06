#pragma once

#include "File/File_stream.hpp"
#include <memory>

namespace rendering
{

class Texture_data_adapter;

namespace texture_storage
{

std::shared_ptr<Texture_data_adapter> load_DDS_texture(file::Input_stream& stream, bool treat_as_linear, std::string& error_message);

}

}
