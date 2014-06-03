#include "Constant_buffer_cache.hpp"
#include "Rendering/Effect/Constant_buffer_adapter.hpp"

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

bool Constant_buffer_cache::connect(Constant_buffer_adapter* adapter, const std::string& name) const
{
	const Handle<Constant_buffer>& buffer = constant_buffer(name);

	if (!buffer)
	{
		return false;
	}

	adapter->set_constant_buffer(buffer);

	return true;
}

void Constant_buffer_cache::set_constant_buffer(const std::string& name, const Handle<Constant_buffer>& buffer)
{
	constant_buffers_[name] = buffer;
}

Handle<Constant_buffer> Constant_buffer_cache::s_null_buffer;

}
