#include "Script_object_wrapper.hpp"
#include "Scripting/Script_engine.hpp"
#include "Scripting/Script_context.hpp"
#include <angelscript.h>

namespace scripting
{

Script_object_wrapper::Script_object_wrapper()
{}

Script_object_wrapper::~Script_object_wrapper()
{}

bool Script_object_wrapper::construct(const std::string& script_type, const std::string& native_type, void* native_object,
									  const Script_engine& engine, Script_context* context, asIScriptModule* module)
{
	asIObjectType* type = engine.engine_->GetObjectTypeById(module->GetTypeIdByDecl(script_type.c_str()));

	if (!type)
	{
		return false;
	}

	// Get the factory function from the object type
	std::string factory_declaration = script_type + " @" + script_type + "(" + native_type + "@)";
	asIScriptFunction* factory = type->GetFactoryByDecl(factory_declaration.c_str());

	if (!factory)
	{
		return false;
	}

	context->context_->Prepare(factory);

	context->context_->SetArgAddress(0, native_object);

	if (context->context_->Execute() < 0)
	{
		return false;
	}

	script_object_ = *(asIScriptObject**)context->context_->GetAddressOfReturnValue();

	if (!script_object_)
	{
		return false;
	}

	script_object_->AddRef();

	on_scene_loaded_ = type->GetMethodByDecl("void on_scene_loaded()");
	on_tick_		 = type->GetMethodByDecl("void on_tick(float)");

	return true;
}

void Script_object_wrapper::clear()
{
	if (script_object_)
	{
		script_object_->Release();
		script_object_ = nullptr;

		on_scene_loaded_ = nullptr;
	}
}

void Script_object_wrapper::execute_on_scene_loaded(Script_context* context)
{
	if (on_scene_loaded_)
	{
		context->context_->Prepare(on_scene_loaded_);
		context->context_->SetObject(script_object_);
		context->context_->Execute();
	}
}

void Script_object_wrapper::execute_on_tick(Script_context* context, float time_slice)
{
	if (on_tick_)
	{
		context->context_->Prepare(on_tick_);
		context->context_->SetObject(script_object_);
		context->context_->SetArgFloat(0, time_slice);
		context->context_->Execute();
	}
}

}
