#include "Scripting.hpp"
#include "scripting_Common.hpp"
#include "scripting_Print.hpp"
#include "scripting_Rendering.hpp"
#include "scripting_Math.hpp"
#include "scripting_Configuration.hpp"
#include "scripting_Scene.hpp"
#include "scripting_Gui.hpp"
#include "../Application/Application.hpp"
#include "../Logging/Logging.hpp"
#include <fstream>
#include <angelscript.h>

namespace scripting
{

void exit();
void build_info();
void resource_info();
void resource_info(const std::string& type);
void mount(const std::string& path);

bool init(Application& application)
{
	app = &application;

	Script_tool& tool = app->script_tool();

	if (!init_print(tool))
	{
		return false;
	}

	if (!init_rendering(tool))
	{
		return false;
	}

	if (!init_math(tool))
	{
		return true;
	}

	if (!init_configuration(tool))
	{
		return false;
	}

	if (!init_scene(tool))
	{
		return false;
	}

	if (!init_gui(tool))
	{
		return false;
	}

	const Script_engine& engine = tool.engine();

	engine.set_default_namespace("");

	engine.register_function("void exit()", asFUNCTION(exit));

	engine.register_function("void build_info()", asFUNCTION(build_info));

	engine.register_function("void resource_info()", asFUNCTIONPR(resource_info, (), void));
	engine.register_function("void resource_info(const string &in)", asFUNCTIONPR(resource_info, (const std::string&), void));

	engine.register_function("void mount(const string &in)", asFUNCTION(mount));

	return true;
}

bool load_console_script(const std::string& name)
{
	std::ifstream stream(name);

	return load_console_script(stream, name);
}

bool load_console_script(std::istream& stream, const std::string& name)
{
	if (!stream)
	{
		logging::error("scripting::load_console_script(): \"" + name + "\" could not be opened.");
		return false;
	}

	logging::post("Loading console script \"" + name + "\":");

	const Script_engine& engine = app->script_tool().engine();

	std::string line;

	while (!stream.eof())
	{
		getline(stream, line, '\n');
		logging::input(line);
		engine.execute_string(line);
	}

	logging::post("End of console script.");

	return true;
}

void exit()
{
	app->request_close();
}

void build_info()
{
	logging::post(app->build_info());
}

void resource_info()
{
	resource_info("");
}

void resource_info(const std::string& type)
{
	std::vector<std::string> info;

	app->resource_manager().get_resource_info(info, type);

	for (auto& i : info)
	{
		logging::post(i);
	}
}

void mount(const std::string& path)
{
	app->resource_manager().virtual_file_system().mount(path);
}

}
