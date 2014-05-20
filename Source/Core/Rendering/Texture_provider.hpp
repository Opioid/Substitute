#pragma once

#include "../Resources/Resource_provider.hpp"
#include "Resources/Handle.hpp"

namespace rendering
{

class Shader_resource_view;
class Rendering_tool;

class Texture_provider : public Resource_provider<Shader_resource_view>
{

public:

	struct Options
	{
		enum Value
		{
			Unknown = 0,
			Treat_as_linear = 1,
			Texture_3D = 2,
			Texture_Cube = 4
		};
	};

	Texture_provider(rendering::Rendering_tool& rendering_tool);

	Handle<Shader_resource_view> load(file::Input_stream& stream, Resource_manager& resource_manager, const Flags flags = Flags()) const;

private:

	rendering::Rendering_tool& rendering_tool_;
};

}


