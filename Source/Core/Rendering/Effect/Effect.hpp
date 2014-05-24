#pragma once

#include "Resources/Resource.hpp"
#include "Resources/Manageable.hpp"
#include "Effect_technique.hpp"
#include "Effect_sampler.hpp"
#include <vector>
#include <map>

namespace rendering
{

class Effect_provider;
class Constant_buffer_adapter;
class Shader_resource_view;
class Sampler_state;

class Effect : public Resource, public Manageable, public Reference_counter
{

public:

	Effect(const std::string& name);
	~Effect();

	void create_default_constant_buffers(Rendering_device& device);

	void use(Rendering_device& device) const;

	Effect_technique* technique(size_t index) const;
	Effect_technique* technique(const std::string& name) const;

	Constant_buffer_adapter* constant_buffer_adapter(const std::string& name) const;

	Effect_sampler& sampler(const std::string& name);
	uint32_t sampler_offset(const std::string& name);

	void apply_sampler_states(Rendering_device& device) const;

private:

	bool buffer_size(const std::string& name, uint32_t& num_bytes) const;

	void set_shader_resource(uint32_t texture_unit, const Handle<Shader_resource_view>& shader_resource);
	void set_sampler_state(uint32_t texture_unit, const Handle<Sampler_state>& sampler_state);

	void apply_samplers(Rendering_device& device) const;

	void add_technique(const std::string& name, Effect_technique* technique);
	void add_constant_buffer_adapter(const std::string& name, Constant_buffer_adapter* adapter);

	void bind_constant_buffers(Rendering_device& device) const;

	std::vector<Effect_technique*>           techniques_;
	std::map<std::string, Effect_technique*> techniques_map_;

	std::vector<Constant_buffer_adapter*> constant_buffer_adapters_;
	std::map<std::string, Constant_buffer_adapter*> constant_buffer_adapters_map_;

	uint32_t num_samplers_;
	Handle<Shader_resource_view>* shader_resources_;
	Handle<Sampler_state>* sampler_states_;

	std::map<std::string, Effect_sampler> samplers_;

	static Effect_sampler  s_invalid_sampler;

	friend Effect_provider;
	friend Effect_technique;
	friend Effect_sampler;
};

}
