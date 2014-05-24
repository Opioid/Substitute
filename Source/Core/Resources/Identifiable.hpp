#pragma once

#include "ID_manager.hpp"
#include <string>

template<typename Type> class Identifiable
{

public:

	Identifiable(const std::string& name) : name_(name)
	{
		s_id = id::manager.id(name);
	}

	const std::string& name() const
	{
		return name_;
	}

	static size_t id()
	{
		return s_id;
	}

private:

	std::string name_;

	static size_t s_id;
};

template<typename Type> size_t Identifiable<Type>::s_id;
