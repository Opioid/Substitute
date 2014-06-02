#pragma once

#include "Identifiable.hpp"
#include "Resources/Handle.hpp"
#include "File/File_stream.hpp"
#include <cstdint>

class Resource_manager;

class Resource_provider_base
{};

template<typename Resource_type>
class Resource_provider : public Resource_provider_base, public Identifiable<Resource_type>
{

public:

	Resource_provider(const std::string& type) : Identifiable<Resource_type>(type)
	{}

	virtual ~Resource_provider()
	{}

	virtual Handle<Resource_type> load(file::Input_stream& stream, Resource_manager& resource_manager, uint32_t flags) const = 0;

    virtual Handle<Resource_type> clone(const Handle<Resource_type>& /*resource*/, Resource_manager& /*resource_manager*/) const
	{
		return nullptr;
	}

    virtual void update(Resource_type* /*resource*/) const
	{}
};
