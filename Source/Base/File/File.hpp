#pragma once

#include <string>

namespace file
{

std::string name_from_path(const std::string& path);

std::string name_without_extension(const std::string& name);

std::string name_with_prefix(const std::string& name, const std::string& prefix);

}
