#include "Script_tool.hpp"
#include <angelscript.h>

namespace scripting
{
	Script_tool::Script_tool(logging::Message_server& server) : Message_sender(server)
	{
		engine_.set_message_callback(&Script_tool::message_callback, this);
	}

	Script_engine const& Script_tool::get_engine() const
	{
		return engine_;
	}

	void Script_tool::message_callback(const std::string& text, Script_engine::Message::Value type) 
	{
		logging::Message::Type::Value message_type;

		switch (type)
		{
		case scripting::Script_engine::Message::Error:
			message_type = logging::Message::Type::Error;
			break;
		case scripting::Script_engine::Message::Warning:
			message_type = logging::Message::Type::Warning;
			break;
		case scripting::Script_engine::Message::Information:
			message_type = logging::Message::Type::Ok;
			break;
		default:
			message_type = logging::Message::Type::Ok;
			break;
		}

		send(text, message_type);
	}
};
