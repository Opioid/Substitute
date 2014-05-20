#pragma once

#include <string>
#include <cstdint>
#include <stack>

namespace file
{

class Virtual_file_system
{

public:

	Virtual_file_system();
	~Virtual_file_system();

	bool is_valid() const;

	void mount(const std::string& path, bool append = true) const;
	void unmount(const std::string& path) const;

	void push_mount(const std::string& path);
	void pop_mount();

	bool file_exists(const std::string& name) const;

	bool get_resolved_path(std::string& resolved_path, const std::string& name) const;
	bool get_resolved_complete_path(std::string& resolved_path, const std::string& name) const;
	bool get_resolved_name(std::string& resolved_name, const std::string& name) const;

	uint64_t file_last_modified(const std::string& name) const;

private:

	bool m_valid;

	std::stack<std::string> m_mount_stack;
};

}
