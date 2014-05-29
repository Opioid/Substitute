#include "Script_engine.hpp"
#include "Script_context.hpp"
#include "scriptstdstring/scriptstdstring.h"
#include "scripthelper/scripthelper.h"
#include <angelscript.h>

namespace scripting
{

Script_engine::Script_engine() //: m_default_module(nullptr)
{
	engine_ = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		
	if (engine_)
	{
		RegisterStdString(engine_);

		engine_->SetMessageCallback(asMETHOD(Script_engine, internal_message_callback), this, asCALL_THISCALL);
	}
}

Script_engine::~Script_engine()
{
	if (engine_)
	{
		engine_->DiscardModule(nullptr);
		engine_->Release();
	}
}

bool Script_engine::is_valid() const
{
	return nullptr != engine_;
}

bool Script_engine::garbage_collect(Garbage_collection flags) const
{
	return engine_->GarbageCollect(static_cast<asDWORD>(flags)) >= 0;
}

Script_context* Script_engine::create_context() const
{
	return new Script_context(engine_->CreateContext());
}

asIScriptModule* Script_engine::module(const std::string& name) const
{
	return engine_->GetModule(name.c_str());
}

bool Script_engine::execute_string(const std::string& script, const Script_context* context) const
{
	if (!context)
	{
		return false;
	}

	int result = ExecuteString(engine_, script.c_str(), nullptr, context->context_);

	if (asEXECUTION_ABORTED == result)
	{
		send_message("Script_engine::execute_string(): Script execution aborted - probably timed out.", Message_type::Error);
	}

	return result >= 0;
}

bool Script_engine::set_default_namespace(const std::string& namespace_name) const
{
	return engine_->SetDefaultNamespace(namespace_name.c_str()) >= 0;
}

bool Script_engine::register_variable(const std::string& name, void* pointer) const
{
	return engine_->RegisterGlobalProperty(name.c_str(), pointer) >= 0;
}

bool Script_engine::register_function(const std::string& name, asSFuncPtr const& function, Calling_convention calling_convention) const
{
	return engine_->RegisterGlobalFunction(name.c_str(), function, static_cast<asDWORD>(calling_convention)) >= 0;
}

bool Script_engine::register_object_type(const std::string& name, int byte_size, int flags) const
{
	return engine_->RegisterObjectType(name.c_str(), byte_size, flags) >= 0;
}

bool Script_engine::register_object_property(const std::string& object, const std::string& declaration, int byte_offset) const
{
	return engine_->RegisterObjectProperty(object.c_str(), declaration.c_str(), byte_offset) >= 0;
}

bool Script_engine::register_object_behavior(const std::string& object, int behavior, const std::string& declaration, asSFuncPtr const& function, Calling_convention calling_convention) const
{
	return engine_->RegisterObjectBehaviour(object.c_str(), asEBehaviours(behavior), declaration.c_str(), function, static_cast<asDWORD>(calling_convention)) >= 0;
}

bool Script_engine::register_object_method(const std::string& object, const std::string& declaration, asSFuncPtr const& function, Calling_convention calling_convention) const
{
	return engine_->RegisterObjectMethod(object.c_str(), declaration.c_str(), function, static_cast<asDWORD>(calling_convention)) >= 0;
}

void Script_engine::internal_message_callback(const asSMessageInfo* message) const
{
	Message_type type;

	switch (message->type)
	{
	case asMSGTYPE_WARNING:
		type = Message_type::Warning;
		break;
	case asMSGTYPE_ERROR:
		type = Message_type::Error;
		break;
	default:
		type = Message_type::Information;
		break;
	}

	send_message(std::string(message->message), type);
}

void Script_engine::send_message(const std::string& text, Message_type type) const
{
	if (message_callback_)
	{
		message_callback_(text, type);
	}
}

}
