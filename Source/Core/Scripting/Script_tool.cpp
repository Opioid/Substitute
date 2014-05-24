#include "Script_tool.hpp"
#include <angelscript.h>

namespace scripting
{

Script_tool::Script_tool(logging::Message_server& server) : Message_sender(server)
{
	engine_.set_message_callback(&Script_tool::message_callback, this);
}

const Script_engine& Script_tool::engine() const
{
	return engine_;
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

};
