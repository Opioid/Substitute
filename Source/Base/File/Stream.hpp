#pragma once

#include <string>
#include <istream>
#include <memory>

namespace file
{

std::unique_ptr<std::istream> open_binary_input_stream(const std::string& name);

std::string read_string(std::istream& stream);

void write_string(std::ostream &stream, const std::string& string);

}
