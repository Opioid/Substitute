#pragma once

#include <string>

struct asIScriptModule;
struct asIScriptObject;

namespace scripting
{

class Script_context;

class Script_object_wrapper
{

public:

	Script_object_wrapper();
	virtual ~Script_object_wrapper();

	bool construct(const std::string& script_type, const std::string& native_type, void* native_object, Script_context& context, asIScriptModule* module);
	void clear();

private:

	void* native_object_;

	asIScriptObject* script_object_;
};

}
