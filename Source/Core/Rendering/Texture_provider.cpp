#include "Texture_provider.hpp"
#include "Rendering_tool.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Texture_data_adapter.hpp"
#include "Rendering/Storage/Texture_storage_load.hpp"
#include "Rendering/Resource_view.hpp"
#include "Flags/Flags.hpp"
#include "Logging/Logging.hpp"

namespace rendering
{

Texture_provider::Texture_provider(rendering::Rendering_tool& rendering_tool) : Resource_provider("Texture"), rendering_tool_(rendering_tool)
{}

Handle<Shader_resource_view> Texture_provider::load(file::Input_stream& stream, Resource_manager& /*resource_manager*/, uint32_t flags) const
{
	std::string error_message;

	Flags<Options, uint32_t> texture_flags(flags);

	bool treat_as_linear = texture_flags.is_set(Options::Treat_as_linear);

	std::shared_ptr<Texture_data_adapter> texture_data = Texture_storage::load_texture(stream, treat_as_linear, error_message);

	if (!texture_data)
	{
		logging::error(error_message);
		return nullptr;
	}

	Handle<Texture> texture;

	switch (texture_data->description().type)
	{
	case Texture_description::Type::Texture_2D:
		texture = rendering_tool_.device().create_texture_2D(*texture_data);
		break;

	case Texture_description::Type::Texture_3D:
		texture = rendering_tool_.device().create_texture_3D(*texture_data);
		break;

	case Texture_description::Type::Texture_cube:
		texture = rendering_tool_.device().create_texture_cube(*texture_data);
		break;

	default:
		return nullptr;
	}

	Handle<Shader_resource_view> view = rendering_tool_.device().create_shader_resource_view(texture, stream.name());

	return view;
}

}
