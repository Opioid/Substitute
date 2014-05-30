#pragma once

#include "Resources/Resource_provider.hpp"
#include "Resources/Handle.hpp"

namespace rendering
{

class Shader_resource_view;
class Rendering_tool;

class Texture_provider : public Resource_provider<Shader_resource_view>
{

public:

	enum class Options
	{
		Unknown			= 1 << 0,
		Treat_as_linear = 1 << 1,
		Texture_3D		= 1 << 2,
		Texture_Cube	= 1 << 3
	};

	Texture_provider(rendering::Rendering_tool& rendering_tool);

	Handle<Shader_resource_view> load(file::Input_stream& stream, Resource_manager& resource_manager, uint32_t flags = 0) const;

private:

	rendering::Rendering_tool& rendering_tool_;
};

}


