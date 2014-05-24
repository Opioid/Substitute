#pragma once

#include "Rendering/Sampler_state.hpp"
#include "Resources/Handle.hpp"
#include <string>

namespace rendering
{

class Shader_resource_view;
class Effect;

class Effect_sampler
{

public:

	Effect_sampler(Effect* effect = nullptr, uint32_t texture_unit = 0xFFFFFFFF);

	bool is_valid() const;

	void set_shader_resource(const Handle<Shader_resource_view>& shader_resource);
	void set_sampler_state(const Handle<Sampler_state>& sampler_state);

	uint32_t texture_unit() const;

	struct Description
	{
		std::string type;
		std::string name;
		Sampler_state::Description state_description;

		static std::string glsl_mapping(const std::string& type);
	};

private:

	Effect* effect_;

	uint32_t texture_unit_;

	friend Effect;
};

}


