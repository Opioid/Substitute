#include "Script_object_wrapper.hpp"
#include "Scripting/Script_context.hpp"
#include <angelscript.h>

namespace scripting
{

Script_object_wrapper::Script_object_wrapper()
{}

Script_object_wrapper::~Script_object_wrapper()
{}

bool Script_object_wrapper::construct(const std::string& script_type, const std::string& native_type, void* native_object, Script_context& context, asIScriptModule* module)
{

}

void Script_object_wrapper::clear()
{
	if (script_object_)
	{
		script_object_->Release();
		script_object_ = nullptr;
	}
}

}
