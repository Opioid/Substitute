#include "scripting_Scene.hpp"
#include "scripting_Common.hpp"
#include "Script_tool.hpp"
#include "Application/Application.hpp"
#include "Logging/Logging.hpp"
#include "String/String.hpp"
#include <angelscript.h>

namespace scripting
{
	bool load_scene(const std::string& name);
	void set_exposure(float exposure);
	void set_linear_white(float white);

	bool init_scene(Script_tool& tool)
	{
		Script_engine const& engine = tool.engine();

		engine.set_default_namespace("scene");

		engine.register_function("bool load(const string &in)", asFUNCTION(load_scene));

		engine.set_default_namespace("camera");

		engine.register_function("void set_exposure(float)", asFUNCTION(set_exposure));
		engine.register_function("void set_linear_white(float)", asFUNCTION(set_linear_white));

		return true;
	}

	bool load_scene(const std::string& name)
	{
		return app->load_scene(name);
	}

	void set_exposure(float exposure)
	{
		app->scene().camera().set_exposure(exposure);
	}

	void set_linear_white(float white)
	{
		app->scene().camera().set_linear_white(float3(white, white, white));
	}

}
