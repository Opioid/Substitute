#pragma once

#include <istream>

class Application;

namespace scripting
{

bool init(Application& application);

bool load_console_script(const std::string& name);
bool load_console_script(std::istream& stream, const std::string& name);

}
