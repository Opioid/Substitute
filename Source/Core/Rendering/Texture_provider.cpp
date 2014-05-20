#include "Texture_provider.hpp"
#include "Rendering_tool.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Storage/Texture_storage_load.hpp"
#include "Rendering/Resource_view.hpp"
#include "../Logging/Logging.hpp"

namespace rendering
{

Texture_provider::Texture_provider(rendering::Rendering_tool& rendering_tool) : Resource_provider("Texture"), rendering_tool_(rendering_tool)
{}

Handle<Shader_resource_view> Texture_provider::load(file::Input_stream& stream, Resource_manager& /*resource_manager*/, const Flags flags) const
{
	std::string error_message;

	bool treat_as_linear = flags.is_set(Options::Treat_as_linear);
	bool texture_3D = flags.is_set(Options::Texture_3D);
	bool texture_cube = flags.is_set(Options::Texture_Cube);

	Handle<Texture> texture;

	if (texture_3D)
	{
		std::shared_ptr<Texture_data_adapter> texture_data = Texture_storage::load_texture_3D(stream, treat_as_linear, error_message);

		if (!texture_data)
		{
			logging::error(error_message);
			return nullptr;
		}

		texture = rendering_tool_.get_device().create_texture_3D(*texture_data);
	}
	else if (texture_cube)
	{
		std::shared_ptr<Texture_data_adapter> texture_data = Texture_storage::load_texture_cube(stream, treat_as_linear, error_message);

		if (!texture_data)
		{
			logging::error(error_message);
			return nullptr;
		}

		texture = rendering_tool_.get_device().create_texture_cube(*texture_data);
	}
	else
	{
		std::shared_ptr<Texture_data_adapter> texture_data = Texture_storage::load_texture_2D(stream, treat_as_linear, error_message);

		if (!texture_data)
		{
			logging::error(error_message);
			return nullptr;
		}

		texture = rendering_tool_.get_device().create_texture_2D(*texture_data);
	}

	Handle<Shader_resource_view> view = rendering_tool_.get_device().create_shader_resource_view(texture, stream.get_name());

	return view;
}

}
