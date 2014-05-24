#pragma once

#include "Scripting/Script_engine.hpp"
#include "Logging/Message_sender.hpp"

namespace scripting
{

class Script_tool : public logging::Message_sender
{

public:

	Script_tool(logging::Message_server& server);

	const Script_engine& engine() const;

	void message_callback(const std::string& text, Script_engine::Message_type type);

private:

	Script_engine engine_;
};

}
