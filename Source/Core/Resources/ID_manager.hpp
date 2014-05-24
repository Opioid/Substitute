#pragma once

#include <vector>
#include <string>

class ID_manager
{

public:

	size_t id(const std::string& type_name);

	std::string name(size_t id) const;

	std::vector<std::string>& get_names() const;
};

namespace id
{
	extern ID_manager manager;
}

