#pragma once

#include <string>

class asIScriptModule;
class asIScriptObject;
class asIScriptFunction;

namespace scripting
{

class Script_engine;
class Script_context;

class Script_object_wrapper
{

public:

	Script_object_wrapper();

	bool construct(const std::string& script_type, const std::string& native_type, void* native_object,
				   const Script_engine& engine, Script_context* context, asIScriptModule* module);

	void clear();

	void execute_on_scene_loaded(Script_context* context);

	void execute_on_tick(Script_context* context, float time_slice);

private:

	asIScriptObject* script_object_;

	asIScriptFunction* on_scene_loaded_;
	asIScriptFunction* on_tick_;
};

}
