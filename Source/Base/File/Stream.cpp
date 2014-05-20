#include "Stream.hpp"
#include <fstream>
#include <cstdint>

namespace file
{
	std::unique_ptr<std::istream> open_binary_input_stream(const std::string& name)
	{
		return std::unique_ptr<std::istream>(new std::ifstream(name.c_str(), std::ios::binary));
	}

	std::string read_string(std::istream& stream)
	{
		uint32_t string_length;
		stream.read((char*)&string_length, sizeof(uint32_t));

		char* string = new char[string_length + 1];

		stream.read((char*)string, sizeof(char) * string_length);
		string[string_length] = 0;

		std::string result(string);

		delete [] string;

		return result;
	}

	void write_string(std::ostream &stream, const std::string& string)
	{
		uint32_t string_length = uint32_t(string.length());
		stream.write((char*)&string_length, sizeof(uint32_t));

		stream.write(string.c_str(), sizeof(char) * string.length());
	}
}
