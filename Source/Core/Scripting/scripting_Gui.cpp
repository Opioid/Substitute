#include "scripting_Scene.hpp"
#include "scripting_Common.hpp"
#include "Script_tool.hpp"
#include "../Application/Application.hpp"
#include <angelscript.h>

namespace scripting
{
//	void image_viewer(const std::string& image_name);

    bool init_gui(Script_tool& /*tool*/)
	{
	//	Script_engine const& engine = tool.engine();

	//	engine.register_function("void image_viewer(const string &in)", asFUNCTION(image_viewer));

		return true;
	}
/*
	void image_viewer(const std::string& image_name)
	{
		Handle<Shader_resource_view> texture = app->resource_manager().load<Shader_resource_view>(image_name);

		if (texture)
		{
			app->get_gui().create_image_viewer(texture, image_name);
		}
	}
	*/
}
