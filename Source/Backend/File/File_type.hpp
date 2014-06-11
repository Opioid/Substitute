#pragma once

#include <istream>

namespace file
{

enum class File_type
{
	Unknown,
	DDS,
	PNG,
	SUI,
	SUM,
	WEBP
};

File_type query_file_type(std::istream& stream);

}


