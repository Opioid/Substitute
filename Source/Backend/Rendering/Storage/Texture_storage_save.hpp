#pragma once

#include "File/File_type.hpp"
#include <string>

namespace rendering
{

class Texture_data_adapter;

namespace texture_storage
{

bool save(const std::string& file_name, const Texture_data_adapter& adapter, file::File_type type);

}

}
