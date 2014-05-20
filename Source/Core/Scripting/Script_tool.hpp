#pragma once

#include "Scripting/Script_engine.hpp"
#include "../Logging/Message_sender.hpp"

namespace scripting
{
	class Script_tool : public logging::Message_sender
	{

	public:

		Script_tool(logging::Message_server& server);

		Script_engine const& get_engine() const;

		void message_callback(const std::string& text, Script_engine::Message::Value type);

	private:

		Script_engine engine_;
	};
}
