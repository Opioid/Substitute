#include "Resource_manager.hpp"
#include "ID_manager.hpp"
#include "Resources/Resource.hpp"

void Resource_manager::release()
{
	for (auto i : stores_)	
	{
		delete i.second;
	}
}

void Resource_manager::cleanup()
{
	bool something_was_released;

	do
	{
		something_was_released = false;

		for (auto i : stores_)	
		{
			if (i.second->release_resources())
			{
				something_was_released = true;
			}
		}
	}
	while (something_was_released);
}

void Resource_manager::reassign_resource_ids()
{
	for (auto i : stores_)	
	{
		i.second->reassign_ids();
	}
}

void Resource_manager::get_resource_info(std::vector<std::string>& info, const std::string& type) const
{
	if (type.empty())
	{
		size_t num_bytes_total = 0;

		info.push_back("");

		for (auto i : stores_)	
		{
			num_bytes_total += i.second->get_resource_info(info);
		}

		info[0] = "Resources, " + string::format_byte_size(num_bytes_total) + ":";
	}
	else
	{
		for (auto i : stores_)	
		{
			if (id::manager.get_name(i.first) == type)
			{
				i.second->get_resource_info(info);
				return;
			}
		}

		info.resize(1);
		info[0] = "No resource of the type \"" + std::string(type) + "\" is registered.";
	}
}

file::Virtual_file_system& Resource_manager::get_virtual_file_system()
{
	return virtual_file_system_;
}
