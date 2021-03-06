#include "Effect.hpp"
#include "Constant_buffer_adapter.hpp"
#include "Rendering/Rendering_device.hpp"
#include "Rendering/Resource_view.hpp"

namespace rendering
{

Effect::Effect(const std::string& name) : Manageable(name), num_samplers_(0), shader_resources_(nullptr), sampler_states_(nullptr)
{}

Effect::~Effect()
{
	for (size_t i = 0; i < techniques_.size(); ++i)
	{
		delete techniques_[i];
	}

	for (size_t i = 0; i < constant_buffer_adapters_.size(); ++i)
	{
		delete constant_buffer_adapters_[i];
	}

	delete [] shader_resources_;
	delete [] sampler_states_;
}

void Effect::create_default_constant_buffers(Rendering_device& device)
{
	for (auto adapter : constant_buffer_adapters_)
	{
		if (!adapter->has_constant_buffer())
		{
			adapter->set_constant_buffer(device.create_constant_buffer(adapter->num_bytes()));
		}
	}
}

void Effect::use(Rendering_device& device) const
{
	bind_constant_buffers(device);

	apply_sampler_states(device);
}

Effect_technique* Effect::technique(size_t index) const
{
	return techniques_[index];
}

Effect_technique* Effect::technique(const std::string& name) const
{
	auto t = techniques_map_.find(name);

	if (techniques_map_.end() == t)
	{
		return nullptr;
	}

	return t->second;
}

Constant_buffer_adapter* Effect::constant_buffer_adapter(const std::string& name) const
{
	auto a = constant_buffer_adapters_map_.find(name);

	if (constant_buffer_adapters_map_.end() == a)
	{
		return nullptr;
	}

	return a->second;
}

Effect_sampler& Effect::sampler(const std::string& name)
{
	auto s = samplers_.find(name);

	if (samplers_.end() == s)
	{
		return s_invalid_sampler;
	}

	return s->second;
}

uint32_t Effect::sampler_offset(const std::string& name)
{
	auto s = samplers_.find(name);

	if (samplers_.end() == s)
	{
		return 0xffffffff;
	}

	return s->second.texture_unit();
}

void Effect::apply_sampler_states(Rendering_device& device) const
{
	device.set_sampler_states(num_samplers_, sampler_states_);
}

bool Effect::buffer_size(const std::string& name, uint32_t& num_bytes) const
{
	for (size_t i = 0; i < techniques_.size(); ++i)
	{
		num_bytes = techniques_[i]->program()->query_constant_buffer_size(name);

		if (0 == num_bytes)
		{
			continue;
		}

		return true;
	}

	return false;
}

void Effect::set_shader_resource(uint32_t texture_unit, const Handle<Shader_resource_view>& shader_resource)
{
	shader_resources_[texture_unit] = shader_resource;
}

void Effect::set_sampler_state(uint32_t texture_unit, const Handle<Sampler_state>& sampler_state)
{
	sampler_states_[texture_unit] = sampler_state;
}

void Effect::apply_samplers(Rendering_device& device) const
{
	device.set_shader_resources(num_samplers_, shader_resources_);
	device.set_sampler_states(num_samplers_, sampler_states_);
}

void Effect::add_technique(const std::string& name, Effect_technique* technique)
{
	techniques_.push_back(technique);
	techniques_map_[name] = technique;
}

void Effect::add_constant_buffer_adapter(const std::string& name, Constant_buffer_adapter* adapter)
{
	constant_buffer_adapters_.push_back(adapter);
	constant_buffer_adapters_map_[name] = adapter;
}

void Effect::bind_constant_buffers(Rendering_device& device) const
{
	for (auto adapter : constant_buffer_adapters_)
	{
//		if (!adapter->is_circular())
//		{
			adapter->bind(device);
//		}
//		else
//		{
//			adapter->restart_circle();
//		}
	}
}

Effect_sampler  Effect::s_invalid_sampler;

}
