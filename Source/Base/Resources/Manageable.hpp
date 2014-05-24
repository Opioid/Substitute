#pragma once

#include <string>
#include <cstdint>

class Resource_manager;

class Manageable
{

public:

	Manageable(const std::string& name);

	const std::string& name() const;
	uint32_t manageable_id() const;
	
protected:

	void set_manageable_id(uint32_t id);

	std::string name_;
	uint32_t    manageable_id_;

	friend Resource_manager;
};
