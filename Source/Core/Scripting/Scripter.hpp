#pragma once

#include "Scripting/Script_builder.hpp"
#include "Memory/Heap_cache.hpp"
#include <string>

namespace scene
{

class Scene;

}

namespace scripting
{

class Script_tool;
class Script_object_wrapper;

class Scripter
{

public:

	Scripter(Script_tool& script_tool);

	void release();

	void clear();

	bool start();

	bool compile();

	bool register_script_class(scene::Scene& scene, const std::string& file_name, const std::string& class_name);

	void execute_on_scene_loaded();

	void execute_on_tick(float time_slice);

private:

	Script_tool& script_tool_;

	Script_builder script_builder_;

	std::string source_buffer_;

	std::set<std::string> added_files_;

	std::string module_name_;

	Heap_cache<Script_object_wrapper> script_objects_;

	struct Registered_object
	{
		std::string script_type;
		std::string native_type;
		void*       native_object;
	};

	std::vector<Registered_object> added_objects_;
};

}
