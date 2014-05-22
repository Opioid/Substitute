#pragma once

#include "Resources/Resource_provider.hpp"
#include "Rendering/Shader_compiler.hpp"
#include "Effect.hpp"
#include "Constant_buffer_description.hpp"
#include "Parsing/Rapidjson_types.hpp"

namespace rendering
{

class Rendering_tool;
class Shader_data_layout;
class Shader_passthrough_layout;

class Effect_provider : public Resource_provider<Effect>
{

public:

	struct Options
	{
		enum Value
		{
			Unknown = 0,
			Use_custom_constant_buffers = 1
		};
	};

	Effect_provider(Rendering_tool& rendering_tool);

	bool load_constant_buffer_classes(const std::string& file_name);

	virtual Handle<Effect> load(file::Input_stream& stream, Resource_manager& resource_manager, const Flags flags = Flags()) const;

private:

	bool load_techniques(Effect& effect, const rapidjson::Value& techniques_node, const Constant_buffer_description_container& constant_buffer_descriptions,
						 const std::vector<Effect_sampler::Description>& sampler_descriptions, const Shader_data_layout& data_layout) const;

	Effect_technique* load_technique(const rapidjson::Value& technique_node, const std::string& constant_buffer_code, const std::string& sampler_code,
									 const Shader_data_layout& data_layout_descriptions) const;

	void load_data_type_descriptions(const rapidjson::Value& layout_node, Shader_data_layout& layout) const;

	void load_sampler_descriptions(const rapidjson::Value& samplers_node, std::vector<Effect_sampler::Description>& descriptions) const;

	void create_constant_buffers(Effect& effect, const Constant_buffer_description_container& constant_buffer_descriptions) const;
	void create_samplers(Effect& effect, const std::vector<Effect_sampler::Description>& descriptions) const;

	bool load_shaders(std::vector<Handle<Shader>>& shaders, const rapidjson::Value& shaders_node, const std::string& constant_buffer_code, const std::string& sampler_code,
					  const std::string& vertex_input_name, const Shader_data_layout& data_layout) const;

	Handle<Shader> compile_shader(Shader::Type type, const std::string& defines,
								  const std::vector<std::string>& sources, const std::string& constant_buffer_code, const std::string& sampler_code,
								  Shader_passthrough_layout& passthrough_layout) const;

	static bool get_shader_source(std::string& source, const std::string& name);

	static std::string generate_sampler_code(const std::vector<Effect_sampler::Description>& description);
	static std::string get_defines_source(const rapidjson::Value& defines);

	static void fill_semantic_mapping(std::map<std::string, std::string>& mapping, const Shader_data_struct& data_struct);

	Constant_buffer_description_cache constant_buffer_description_cache_;

	Shader_compiler shader_compiler_;

	Rendering_tool& rendering_tool_;
};

}
