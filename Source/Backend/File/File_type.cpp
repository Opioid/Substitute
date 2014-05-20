#include "File_type.hpp"
#include <cstring>

namespace file
{

File_type get_file_type(std::istream& stream)
{
	stream.clear();
	stream.seekg(0, std::ios_base::beg);

	if (!stream)
	{
		return File_type::Unknown;
	}

	char header[3];
	stream.read(header, sizeof(char) * 3);

	stream.seekg(0, std::ios_base::beg);

	File_type file_type = File_type::Unknown;

	if (!strncmp("DDS", header, 3))
	{
		file_type = File_type::DDS;
	}
	else if (!strncmp("SUI", header, 3))
	{
		file_type = File_type::SUI;
	}
	else if (!strncmp("SUM", header, 3))
	{
		file_type = File_type::SUM;
	}

	return file_type;
}

}
