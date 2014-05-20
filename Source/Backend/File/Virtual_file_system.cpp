#include "Virtual_file_system.hpp"
#include <physfs.h>

namespace file
{

Virtual_file_system::Virtual_file_system() : m_valid(PHYSFS_init(nullptr) != 0)
{}

Virtual_file_system::~Virtual_file_system()
{
	PHYSFS_deinit();
}

bool Virtual_file_system::is_valid() const
{
	return m_valid;
}

void Virtual_file_system::mount(const std::string& path, bool append) const
{
	PHYSFS_mount(path.c_str(), nullptr, append ? 1 : 0);
}

void Virtual_file_system::unmount(const std::string& path) const
{
	PHYSFS_removeFromSearchPath(path.c_str());
}

void Virtual_file_system::push_mount(const std::string& path)
{
	mount(path);

	m_mount_stack.push(path);
}

void Virtual_file_system::pop_mount()
{
	if (m_mount_stack.empty())
	{
		return;
	}

	const std::string& path = m_mount_stack.top();

	unmount(path);

	m_mount_stack.pop();
}

bool Virtual_file_system::file_exists(const std::string& name) const
{
	return PHYSFS_exists(name.c_str()) != 0;
}

bool Virtual_file_system::get_resolved_path(std::string& resolved_path, const std::string& name) const
{
	const char* resolved = PHYSFS_getRealDir(name.c_str());

	if (!resolved)
	{
		return false;
	}

	resolved_path = std::string(resolved);

	return true;
}

bool Virtual_file_system::get_resolved_complete_path(std::string& resolved_path, const std::string& name) const
{
	if (!get_resolved_path(resolved_path, name))
	{
		return false;
	}

	resolved_path += "/" + name.substr(0, name.find_last_of("/\\"));

	return true;
}

bool Virtual_file_system::get_resolved_name(std::string& resolved_name, const std::string& name) const
{
	if (!get_resolved_path(resolved_name, name))
	{
		return false;
	}

	resolved_name += "/" + name;

	return true;
}

uint64_t Virtual_file_system::file_last_modified(const std::string& name) const
{
	PHYSFS_sint64 timestamp = PHYSFS_getLastModTime(name.c_str());

	if (timestamp < 0)
	{
		return 0;
	}
	else
	{
		return uint64_t(timestamp);
	}
}

}
