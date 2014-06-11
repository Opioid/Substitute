#pragma once

#include <string>

namespace rendering
{

class Texture_data_adapter;

namespace texture_storage
{

bool save_WEBP(const std::string& file_name, const Texture_data_adapter& adapter);

}

}
