#include "Scripter.hpp"
#include "Script_tool.hpp"
#include "Script_object_wrapper.hpp"
#include "Scripting/Script_context.hpp"
#include "File/Text_stream.hpp"
#include "Logging/Logging.hpp"

namespace scripting
{

Scripter::Scripter(Script_tool& script_tool) : script_tool_(script_tool), module_name_("Scene")
{}

void Scripter::release()
{
	clear();
}

void Scripter::clear()
{
	for (auto o : script_objects_)
	{
		o->clear();
	}

	script_objects_.clear();

	added_files_.clear();

	asIScriptModule* module = script_tool_.engine().module(module_name_);

	if (module)
	{
		module->Discard();
	}
}

bool Scripter::start()
{
	clear();

	return script_builder_.start_new_module(module_name_, script_tool_.engine());
}

bool Scripter::compile()
{
	if (!script_builder_.compile_module())
	{
		return false;
	}

	auto context = script_tool_.main_context();

	asIScriptModule* module = script_tool_.engine().module(module_name_);

	for (auto& added_object : added_objects_)
	{
		Script_object_wrapper* object = script_objects_.add();

		if (!object->construct(added_object.script_type, added_object.native_type, added_object.native_object, script_tool_.engine(), context, module))
		{
			script_objects_.pop();
		}
	}

	added_objects_.clear();

	return true;
}

bool Scripter::register_script_class(scene::Scene& scene, const std::string& file_name, const std::string& class_name)
{
	return register_script_class(static_cast<void*>(&scene), "Scene", file_name, class_name);
}

bool Scripter::register_script_class(scene::Light& light, const std::string& file_name, const std::string& class_name)
{
	return register_script_class(static_cast<void*>(&light), "Light", file_name, class_name);
}

bool Scripter::register_script_class(scene::Actor& actor, const std::string& file_name, const std::string& class_name)
{
	return register_script_class(static_cast<void*>(&actor), "Actor", file_name, class_name);
}

void Scripter::execute_on_scene_loaded()
{
	auto context = script_tool_.main_context();

	for (auto o : script_objects_)
	{
		o->execute_on_scene_loaded(context);
	}
}

void Scripter::execute_on_tick(float time_slice)
{
	auto context = script_tool_.main_context();

	for (auto o : script_objects_)
	{
		o->execute_on_tick(context, time_slice);
	}
}

bool Scripter::register_script_class(void* native_object, const std::string& native_type, const std::string& file_name, const std::string& class_name)
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
		logging::error("Scripter::register_script_class(): could not open add section for \"" + file_name + "\".");

		return false;
	}

	added_objects_.push_back( { class_name, native_type, native_object } );

	return true;
}

}
