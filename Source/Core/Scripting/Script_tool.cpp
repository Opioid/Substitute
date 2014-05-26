#include "Script_tool.hpp"
#include "Scripting/Script_context.hpp"
#include <angelscript.h>

namespace scripting
{

Script_tool::Script_tool(logging::Message_server& server) : Message_sender(server)
{
	engine_.set_message_callback(&Script_tool::message_callback, this);

	default_context_ = engine_.create_context();
}

void Script_tool::release()
{
	delete default_context_;
}

const Script_engine& Script_tool::engine() const
{
	return engine_;
}

Script_context* Script_tool::default_context()
{
	return default_context_;
}

void Script_tool::execute_string(const std::string& script) const
{
	engine_.execute_string(script, default_context_);
}

void Script_tool::message_callback(const std::string& text, Script_engine::Message_type type)
{
	logging::Message::Type message_type;

	switch (type)
	{
	case Script_engine::Message_type::Error:
		message_type = logging::Message::Type::Error;
		break;
	case Script_engine::Message_type::Warning:
		message_type = logging::Message::Type::Warning;
		break;
	case Script_engine::Message_type::Information:
		message_type = logging::Message::Type::Ok;
		break;
	default:
		message_type = logging::Message::Type::Ok;
		break;
	}

	send(text, message_type);
}

}
