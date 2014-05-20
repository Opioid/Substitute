#include "ID_manager.hpp"

namespace id
{
	ID_manager manager;
}

size_t ID_manager::get_id(const std::string& type_name)
{
	for (size_t i = 0; i < get_names().size(); ++i)
	{
		if (get_names()[i] == type_name) 
		{
			return i;
		}
	}

	get_names().push_back(type_name);

	return get_names().size() - 1;
}

std::string ID_manager::get_name(size_t id) const
{
	return get_names()[id];
}

std::vector<std::string>& ID_manager::get_names() const
{
	static std::vector<std::string> type_names;
	return type_names;
}


