#include "Effect_provider.hpp"
#include "Shader_data_layout.hpp"
#include "Shader_passthrough_layout.hpp"
#include "Constant_buffer_adapter.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Rendering_tool.hpp"
#include "Resources/Resource_manager.hpp"
#include "File/Text_stream.hpp"
#include "Parsing/Json.hpp"
#include "Flags/Flags.hpp"
#include "Logging/Logging.hpp"
#include <sstream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <iostream>

namespace rendering
{

Effect_provider::Effect_provider(Rendering_tool& rendering_tool) : Resource_provider("Effect"), shader_compiler_(rendering_tool.device()), rendering_tool_(rendering_tool)
{}

bool Effect_provider::load_constant_buffer_classes(const std::string& file_name)
{
	file::Input_stream stream(file_name);

	if (!stream)
	{
		return false;
	}

	return constant_buffer_description_cache_.load(stream);
}

Handle<Effect> Effect_provider::load(file::Input_stream& stream, Resource_manager& /*resource_manager*/, uint32_t flags) const
{
	rapidjson::Document root;

	if (!json::parse(root, stream))
	{
		logging::error("Effect_provider::load(): \"" + stream.name() + "\" could not be loaded: failed to parse the file: " + json::read_error(root, stream));

		return nullptr;
	}

	rapidjson::Value* techniques_node = nullptr;

	Constant_buffer_description_container constant_buffer_descriptions;
	Shader_data_layout data_layout_descriptions;
	std::vector<Effect_sampler::Description> sampler_descriptions;

	for (auto n = root.MemberBegin(); n != root.MemberEnd(); ++n)
	{
		const std::string node_name = n->name.GetString();
		const rapidjson::Value& node_value = n->value;

		if ("techniques" == node_name)
		{
			techniques_node = &n->value;
		}
		else if ("buffers" == node_name)
		{
			Constant_buffer_description_container::load(constant_buffer_descriptions, node_value);

			for (auto& d : constant_buffer_descriptions.descriptions)
			{
				if (d.elements.empty())
				{
					constant_buffer_description_cache_.fill_description_from_class(d);
				}
			}
		}
		else if ("data_layouts" == node_name)
		{
			load_data_type_descriptions(node_value, data_layout_descriptions);
		}
		else if ("samplers" == node_name)
		{
			load_sampler_descriptions(node_value, sampler_descriptions);
		}
	}

	if (!techniques_node)
	{
		return nullptr;
	}

	Handle<Effect> effect(new Effect(stream.name()));

	if (!load_techniques(*effect, *techniques_node, constant_buffer_descriptions, sampler_descriptions, data_layout_descriptions))
	{
		return nullptr;
	}

	create_constant_buffers(*effect, constant_buffer_descriptions);
	create_samplers(*effect, sampler_descriptions);

	Flags<Options> effect_flags(flags);

	if (!effect_flags.is_set(Options::Use_custom_constant_buffers))
	{
		effect->create_default_constant_buffers(rendering_tool_.device());
	}

	return effect;
}

bool Effect_provider::load_techniques(Effect& effect, const rapidjson::Value& techniques_node, const Constant_buffer_description_container& constant_buffer_descriptions,
									  const std::vector<Effect_sampler::Description>& sampler_descriptions, const Shader_data_layout& data_layout_descriptions) const
{
	if (!techniques_node.IsArray())
	{
		return false;
	}

	std::string constant_buffer_code = constant_buffer_descriptions.generate_constant_buffers_code();
	std::string sampler_code = generate_sampler_code(sampler_descriptions);

	for (rapidjson::SizeType i = 0; i < techniques_node.Size(); ++i)
	{
		const rapidjson::Value& node = techniques_node[i];

		const rapidjson::Value::Member* name_node = node.FindMember("name");

		if (!name_node)
		{
			continue;
		}

		const std::string name = name_node->value.GetString();

		Effect_technique* technique = load_technique(node, constant_buffer_code, sampler_code, data_layout_descriptions);

		if (technique)
		{
			effect.add_technique(name, technique);
		}
		else
		{
			return false;
		}
	}

	return true;
}

Effect_technique* Effect_provider::load_technique(const rapidjson::Value& technique_node, const std::string& constant_buffer_code,
												  const std::string& sampler_code, const Shader_data_layout& data_layout_descriptions) const
{
	Effect_technique* technique = new Effect_technique(rendering_tool_.device().create_shader_program());

	std::string vertex_input_name;

	const rapidjson::Value* shaders_node;

	for (auto n = technique_node.MemberBegin(); n != technique_node.MemberEnd(); ++n)
	{
		const std::string node_name = n->name.GetString();
		const rapidjson::Value& node_value = n->value;

		if (node_name== "input")
		{
			vertex_input_name = node_value.GetString();
		}
		else if (node_name == "shaders")
		{
			shaders_node = &node_value;
		}
	}

	std::vector<Handle<Shader>> shaders;

	if (!load_shaders(shaders, *shaders_node, constant_buffer_code, sampler_code, vertex_input_name, data_layout_descriptions))
	{
		delete technique;

		return nullptr;
	}

	std::map<std::string, std::string> semantic_mapping;

	// Vertex input layout
	const auto& descriptions = data_layout_descriptions.descriptions();
	auto vi = descriptions.find(vertex_input_name);

	if (descriptions.end() != vi)
	{
		fill_semantic_mapping(semantic_mapping, vi->second);
	}

	std::string error_message;

	if (!technique->program()->link(shaders, error_message, &semantic_mapping))
	{
		logging::error("Effect_provider::load_technique(): " + error_message);
		delete technique;
		return nullptr;
	}

	return technique;
}

void Effect_provider::load_data_type_descriptions(const rapidjson::Value& layout_node, Shader_data_layout& layout) const
{
	if (!layout_node.IsArray())
	{
		return;
	}

	for (rapidjson::SizeType i = 0; i < layout_node.Size(); ++i)
	{
		Shader_data_struct data_struct;

		if (Shader_data_struct::load(data_struct, layout_node[i]))
		{
			if (!data_struct.elements.empty())
			{
				layout.add_description(data_struct.name, data_struct);
			}
		}
	}
}

void Effect_provider::load_sampler_descriptions(const rapidjson::Value& samplers_node, std::vector<Effect_sampler::Description>& descriptions) const
{
	if (!samplers_node.IsArray())
	{
		return;
	}

	for (rapidjson::SizeType i = 0; i < samplers_node.Size(); ++i)
	{
		const rapidjson::Value& sampler_node = samplers_node[i];

		Effect_sampler::Description description;

		for (auto n = sampler_node.MemberBegin(); n != sampler_node.MemberEnd(); ++n)
		{
			const std::string node_name = n->name.GetString();
			const rapidjson::Value& node_value = n->value;

			if ("type" == node_name)
			{
				description.type = node_value.GetString();
			}
			else if ("name" == node_name)
			{
				description.name = node_value.GetString();
			}
			else if ("filtering" == node_name)
			{
				std::string filtering = node_value.GetString();

				if ("Point" == filtering)
				{
					description.state_description.filtering = Sampler_state::Description::Filtering::Point;
				}
				else if ("Bilinear" == filtering)
				{
					description.state_description.filtering = Sampler_state::Description::Filtering::Bilinear;
				}
				else if ("Trilinear" == filtering)
				{
					description.state_description.filtering = Sampler_state::Description::Filtering::Trilinear;
				}
				else if ("Anisotropic" == filtering)
				{
					description.state_description.filtering = Sampler_state::Description::Filtering::Anisotropic;

					if (1 <= description.state_description.max_anisotropy)
					{
						description.state_description.max_anisotropy = 8;
					}
				}
			}
			else if (node_name == "max_anisotropy")
			{
				description.state_description.max_anisotropy = node_value.GetUint();
			}
			else if (node_name == "address")
			{
				std::string address = node_value.GetString();

				if ("Repeat" == address)
				{
					description.state_description.address = Sampler_state::Description::Address::Repeat;
				}
				else if ("Clamp" == address)
				{
					description.state_description.address = Sampler_state::Description::Address::Clamp;
				}
			}
		}

		descriptions.push_back(description);
	}
}

void Effect_provider::create_constant_buffers(Effect& effect, const Constant_buffer_description_container& constant_buffer_descriptions) const
{
	uint32_t binding_point = 0;

	for (auto& buffer : constant_buffer_descriptions.descriptions)
	{
		uint32_t num_bytes;
		if (!effect.buffer_size(buffer.name, num_bytes))
		{
			continue;
		}

	//	uint32_t allocate_size = buffer.circular_size == 1 ? num_bytes : Constant_buffer_adapter::calculate_circle_segment_size(num_bytes) * buffer.circular_size;

		Constant_buffer_adapter* adapter = new Constant_buffer_adapter(binding_point, num_bytes/*allocate_size, buffer.circular_size*/);

		effect.add_constant_buffer_adapter(buffer.name, adapter);

		++binding_point;
	}
}

void Effect_provider::create_samplers(Effect& effect, const std::vector<Effect_sampler::Description>& descriptions) const
{
	effect.num_samplers_ = uint32_t(descriptions.size());

	effect.shader_resources_ = new Handle<Shader_resource_view>[effect.num_samplers_];
	effect.sampler_states_ = new Handle<Sampler_state>[effect.num_samplers_];

	for (size_t i = 0; i < descriptions.size(); ++i)
	{
		effect.samplers_[descriptions[i].name] = Effect_sampler(&effect, uint32_t(i));

		effect.sampler_states_[i] = rendering_tool_.sampler_state_cache().get_sampler_state(descriptions[i].state_description);
	}
}

bool Effect_provider::load_shaders(std::vector<Handle<Shader>>& shaders, const rapidjson::Value& shaders_node, const std::string& constant_buffer_code, const std::string& sampler_code,
								   const std::string& vertex_input_name, const Shader_data_layout& data_layout) const
{
	if (!shaders_node.IsArray())
	{
		return false;
	}

	struct Shader_node
	{
		Shader_node(Shader::Type type, const rapidjson::Value* node) : type(type), node(node)
		{}

		Shader::Type type;
		const rapidjson::Value* node;
	};

	std::vector<Shader_node> shader_nodes;

	for (rapidjson::SizeType i = 0; i < shaders_node.Size(); ++i)
	{
		const rapidjson::Value& node = shaders_node[i];

		const rapidjson::Value::Member* type_node = node.FindMember("type");

		if (!type_node)
		{
			continue;
		}

		std::string type_name = type_node->value.GetString();

		Shader::Type type = Shader::Type::Unknown;

		if ("vertex" == type_name)
		{
			type = Shader::Type::Vertex;
		}
		else if ("geometry" == type_name)
		{
			type = Shader::Type::Geometry;
		}
		else if ("pixel" == type_name)
		{
			type = Shader::Type::Pixel;
		}

		if (Shader::Type::Unknown == type)
		{
			continue;
		}

		shader_nodes.push_back(Shader_node(type, &node));
	}

	std::sort(shader_nodes.begin(), shader_nodes.end(), [](const Shader_node& a, const Shader_node& b) -> bool { return a.type < b.type; } );

	Shader_passthrough_layout passthrough_layout(data_layout);

	std::string previous_output_name;

	for (size_t i = 0; i < shader_nodes.size(); ++i)
	{
		const auto& s = shader_nodes[i];

		const rapidjson::Value::Member* sources_node = s.node->FindMember("sources");

		if (!sources_node || !sources_node->value.IsArray())
		{
			return false;
		}

		const rapidjson::Value& sources_value = sources_node->value;

		std::vector<std::string> sources;

		for (rapidjson::SizeType j = 0; j < sources_value.Size(); ++j)
		{
			const auto& source_node = sources_value[j];

			std::string source_name = source_node.GetString();
			std::string source;
			bool result = file::read_text_file(source_name, source);

			if (!result)
			{
				logging::error("Effect_provider::load_shaders(): could not load \"" + source_name + "\".");
				return false;
			}

			if (Shader::Type::Vertex == s.type)
			{
				boost::replace_all(source, "vertex_in.", "vertex_in_");
			}
			else if (Shader::Type::Pixel == s.type)
			{
				boost::replace_all(source, "pixel_out.", "pixel_out_");
			}

			sources.push_back(source);
		}

		passthrough_layout.set_input_name(i == 0 ? vertex_input_name : previous_output_name);

		std::string output_name = json::read_string(*s.node, "output", "");

		passthrough_layout.set_output_name(output_name);

		previous_output_name = output_name;

		if (Shader::Type::Geometry == s.type)
		{
			passthrough_layout.set_input_primitive_topology(json::read_string(*s.node, "input_primitive_topology", ""));
			passthrough_layout.set_output_primitive_topology(json::read_string(*s.node, "output_primitive_topology", ""));
			passthrough_layout.set_max_vertex_count(json::read_uint(*s.node, "max_vertex_count", 0));
		}

		const rapidjson::Value::Member* defines_node = s.node->FindMember("defines");

		std::string defines = defines_node ? get_defines_source(defines_node->value) : "";

		Handle<Shader> shader = compile_shader(s.type, defines, sources, constant_buffer_code, sampler_code, passthrough_layout);

		if (!shader)
		{
			return false;
		}

		shaders.push_back(shader);
	}

	return true;
}

Handle<Shader> Effect_provider::compile_shader(Shader::Type type, const std::string& defines,
											   const std::vector<std::string>& sources, const std::string& constant_buffer_code, const std::string& sampler_code,
											   Shader_passthrough_layout& passthrough_layout) const
{
	std::ostringstream patched_source;

	patched_source << "#version 430" << std::endl;

	patched_source << "#define saturate(a) clamp(a, 0.f, 1.f)" << std::endl;
	patched_source << "#define lerp(a, b, c) mix(a, b, c)" << std::endl;
	patched_source << "#define log10(a) (log2(a) / log2(10.f))" << std::endl;
	patched_source << defines << std::endl;

	patched_source << constant_buffer_code << std::endl;
	patched_source << sampler_code << std::endl;

	patched_source << passthrough_layout.generate_input_layout_code(type) << std::endl;
	patched_source << passthrough_layout.generate_output_layout_code(type) << std::endl;

	for (size_t i = 0; i < sources.size(); ++i)
	{
		patched_source << std::endl << sources[i];
	}

	if (Shader::Type::Geometry == type)
	{
		std::ofstream writer("../../Cache/Effect.txt");
		writer << patched_source.str();
	}

	std::string error_message;
	Handle<Shader> shader = shader_compiler_.compile(type, patched_source.str(), error_message);

	if (!shader)
	{
		logging::error("Effect_provider::load_shaders(): " + error_message);
	}

	return shader;
}

std::string Effect_provider::generate_sampler_code(const std::vector<Effect_sampler::Description>& description)
{
	if (description.empty())
	{
		return "";
	}

	std::ostringstream code;

	for (size_t i = 0; i < description.size(); ++i)
	{
		code << "layout(binding = " << i << ") uniform " << Effect_sampler::Description::glsl_mapping(description[i].type) << " " << description[i].name << ";" << std::endl;
	}

	return code.str();
}

std::string Effect_provider::get_defines_source(const rapidjson::Value& defines)
{
	std::stringstream source;

	if (defines.IsArray())
	{
		for (rapidjson::SizeType i = 0; i < defines.Size(); ++i)
		{
			source << "#define " << defines[i].GetString() << std::endl;
		}
	}

	return source.str();
}

void Effect_provider::fill_semantic_mapping(std::map<std::string, std::string>& mapping, const Shader_data_struct& data_struct)
{
	for (size_t i = 0; i < data_struct.elements.size(); ++i)
	{
		mapping["vertex_in_" + data_struct.elements[i].name] = data_struct.elements[i].semantic;
	}
}

}
