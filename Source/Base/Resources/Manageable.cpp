#include "Manageable.hpp"

Manageable::Manageable(const std::string& name) : name_(name), m_manageable_id(0)
{}

const std::string& Manageable::get_name() const
{
	return name_;
}

uint32_t Manageable::get_manageable_id() const
{
	return m_manageable_id;
}

void Manageable::set_manageable_id(uint32_t id)
{
	m_manageable_id = id;
}
