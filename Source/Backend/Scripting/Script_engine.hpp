#pragma once

#include <string>
#include <functional>
#include <cstdint>

class asIScriptEngine;
class asIScriptModule;
struct asSMessageInfo;
struct asSFuncPtr;

namespace scripting
{

class Script_context;
class Script_builder;

class Script_engine
{

public:

	enum class Message_type
	{
		Error,
		Warning,
		Information
	};

	enum class Calling_convention
	{
		C_decl = 0,
		Std_call,
		This_call_as_global,
		This_call,
		C_decl_obj_last,
		C_decl_obj_first,
		Generic
	};

	enum class Garbage_collection
	{
		Full_cycle      = 1,
		One_step        = 2,
		Destroy_garbage = 4,
		Detect_garbage  = 8
	};

	Script_engine();
	~Script_engine();

	bool is_valid() const;

	template<typename T>
	void set_message_callback(void (T::*mp)(const std::string&, Message_type), T* callee)
	{
		message_callback_ = std::bind(mp, callee, std::placeholders::_1, std::placeholders::_2);
	}

	bool garbage_collect(Garbage_collection flags = Garbage_collection::Full_cycle) const;

	Script_context* create_context() const;

	asIScriptModule* module(const std::string& name) const;

	bool execute_string(const std::string& script, const Script_context* context) const;

	bool set_default_namespace(const std::string& namespace_name) const;

	bool register_variable(const std::string& name, void* pointer) const;

	bool register_function(const std::string& name, const asSFuncPtr& function, Calling_convention calling_convention = Calling_convention::C_decl) const;

	bool register_object_type(const std::string& name, int byte_size, int flags) const;

	bool register_object_property(const std::string& object, const std::string& declaration, int byte_offset) const;

	bool register_object_behavior(const std::string& object, int behavior, const std::string& declaration, const asSFuncPtr& function,
								  Calling_convention calling_convention) const;

	bool register_object_method(const std::string& object, const std::string& declaration, const asSFuncPtr& function,
								Calling_convention calling_convention = Calling_convention::This_call) const;

//private:

	void internal_message_callback(const asSMessageInfo* message_info) const;

	void send_message(const std::string& text, Message_type type) const;

	asIScriptEngine*  engine_;
//		asIScriptModule*  m_default_module;

	std::function<void(const std::string&, Message_type)> message_callback_;

	friend Script_builder;
};

}
