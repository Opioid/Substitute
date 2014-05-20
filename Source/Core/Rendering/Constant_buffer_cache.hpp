#pragma once

#include "Resources/Handle.hpp"
#include "Rendering/Data_buffer.hpp"
#include <string>
#include <map>

namespace rendering
{

class Constant_buffer_cache
{

public:

	const Handle<Constant_buffer>& get_constant_buffer(const std::string& name) const;

	void set_constant_buffer(const std::string& name, const Handle<Constant_buffer>& buffer);

private:

	std::map<std::string, Handle<Constant_buffer>> constant_buffers_;

	static Handle<Constant_buffer> s_null_buffer;
};

}
