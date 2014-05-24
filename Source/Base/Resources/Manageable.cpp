#include "Manageable.hpp"

Manageable::Manageable(const std::string& name) : name_(name), manageable_id_(0)
{}

const std::string& Manageable::name() const
{
	return name_;
}

uint32_t Manageable::manageable_id() const
{
	return manageable_id_;
}

void Manageable::set_manageable_id(uint32_t id)
{
	manageable_id_ = id;
}
