#pragma once

#include "Scripting/Script_builder.hpp"
#include <string>

namespace scene
{

class Scene;

}

namespace scripting
{

class Script_tool;

class Scripter
{

public:

	Scripter(Script_tool& script_tool);

	bool start();

	bool compile();

	bool register_script_class(scene::Scene& scene, const std::string& file_name, const std::string& class_name);

	void execute_on_scene_loaded();

	void execute_on_tick();

private:

	Script_tool& script_tool_;

	Script_builder script_builder_;

	std::string source_buffer_;

	std::set<std::string> added_files_;

	std::string module_name_;
};

}
