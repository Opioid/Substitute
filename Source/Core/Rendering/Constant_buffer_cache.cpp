#include "Constant_buffer_cache.hpp"

namespace rendering
{

const Handle<Constant_buffer>& Constant_buffer_cache::constant_buffer(const std::string& name) const
{
	auto b = constant_buffers_.find(name);

	if (constant_buffers_.end() == b)
	{
		return s_null_buffer;
	}

	return b->second;
}

void Constant_buffer_cache::set_constant_buffer(const std::string& name, const Handle<Constant_buffer>& buffer)
{
	constant_buffers_[name] = buffer;
}

Handle<Constant_buffer> Constant_buffer_cache::s_null_buffer;

}
