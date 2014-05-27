#include "Scripter.hpp"
#include "Script_tool.hpp"
#include "Scripting/Script_context.hpp"
#include "File/Text_stream.hpp"
#include "Logging/Logging.hpp"

namespace scripting
{

Scripter::Scripter(Script_tool& script_tool) : script_tool_(script_tool), module_name_("Scene")
{

}

bool Scripter::start()
{
	added_files_.clear();

	return script_builder_.start_new_module(module_name_, script_tool_.engine());
}

bool Scripter::compile()
{
	if (!script_builder_.compile_module())
	{
		return false;
	}

	return true;
}

bool Scripter::register_script_class(scene::Scene& scene, const std::string& file_name, const std::string& class_name)
{
	if (file_name.empty() || class_name.empty())
	{
		return false;
	}

	if (added_files_.end() != added_files_.find(file_name))
	{
		return true;
	}

	source_buffer_.clear();

	if (!file::read_text_file(file_name, source_buffer_))
	{
		logging::error("Scripter::register_script_class(): could not open stream for \"" + file_name + "\".");

		return false;
	}

	if (!script_builder_.add_section(file_name, source_buffer_))
	{
	//	logging::error()

		return false;
	}

	return true;
}

void Scripter::execute_on_scene_loaded()
{
	auto* context = script_tool_.default_context()->context_;

	asIScriptModule* module = script_tool_.engine().query_module(module_name_);

	asIObjectType* type = script_tool_.engine().engine_->GetObjectTypeById(module->GetTypeIdByDecl("Interpolation_test"));

	// Get the factory function from the object type
	asIScriptFunction* factory = type->GetFactoryByDecl("Interpolation_test @Interpolation_test()");
	// Prepare the context to call the factory function

	if (!factory)
	{
		logging::error("Oh no");
		return;
	}

	context->Prepare(factory);
	// Execute the callr
	context->Execute();
	// Get the object that was created
	asIScriptObject* object = *(asIScriptObject**)context->GetAddressOfReturnValue();
	// If you're going to store the object you must increase the reference,
	// otherwise it will be destroyed when the context is reused or destroyed.
	object->AddRef();

	// Obtain the function object that represents the class method
	asIScriptFunction* function = type->GetMethodByDecl("void on_scene_loaded()");
	// Prepare the context for calling the method
	context->Prepare(function);
	// Set the object pointer
	context->SetObject(object);
	// Execute the call
	context->Execute();
/*
	asIScriptFunction* function = module->GetFunctionByDecl("void on_scene_loaded()");
	context->Prepare(function);
	context->Execute();
	*/
}

void Scripter::execute_on_tick()
{

}

}
