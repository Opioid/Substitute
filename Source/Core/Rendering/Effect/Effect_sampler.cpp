#include "Effect_sampler.hpp"
#include "Effect.hpp"

namespace rendering
{

Effect_sampler::Effect_sampler(Effect* effect, uint32_t texture_unit) : effect_(effect), texture_unit_(texture_unit)
{}

bool Effect_sampler::is_valid() const
{
	return texture_unit_ != 0xFFFFFFFF;
}

void Effect_sampler::set_shader_resource(const Handle<Shader_resource_view>& shader_resource)
{
	effect_->set_shader_resource(texture_unit_, shader_resource);
}

void Effect_sampler::set_sampler_state(const Handle<Sampler_state>& sampler_state)
{
	effect_->set_sampler_state(texture_unit_, sampler_state);
}

uint32_t Effect_sampler::texture_unit() const
{
	return texture_unit_;
}

std::string Effect_sampler::Description::glsl_mapping(const std::string& type)
{
	static std::map<std::string, std::string> mapping;

	if (mapping.empty())
	{
		mapping["2D"]             = "sampler2D";
		mapping["2D_multisample"] = "sampler2DMS";
		mapping["2D_array"]       = "sampler2DArray";
		mapping["3D"]             = "sampler3D";
		mapping["Cube"]           = "samplerCube";
	}

	auto t = mapping.find(type);

	if (mapping.end() != t)
	{
		return t->second;
	}

	return "unknown";
}

}
