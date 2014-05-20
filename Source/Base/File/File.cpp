#include "File.hpp"

namespace file
{

std::string name_from_path(const std::string& path)
{
	size_t found = path.find_last_of("/\\");

	return path.substr(found + 1);
}

std::string name_without_extension(const std::string& name)
{
	size_t found = name.find_last_of(".");

	return name.substr(0, found);
}

std::string name_with_prefix(const std::string& name, const std::string& prefix)
{
	size_t found = name.find_last_of("/\\");

	if (std::string::npos == found)
	{
		return name;
	}

	std::string copy = name;

	copy.insert(found + 1, prefix);

	return copy;
}

}
