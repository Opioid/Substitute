#pragma once

#include "Scripting/Script_engine.hpp"
#include "Logging/Message_sender.hpp"

namespace scripting
{

class Script_tool : public logging::Message_sender
{

public:

	Script_tool(logging::Message_server& server);

	void release();

	const Script_engine& engine() const;

	Script_context* console_context();

	Script_context* main_context();

	void execute_string(const std::string& script) const;

	void message_callback(const std::string& text, Script_engine::Message_type type);

private:

	Script_engine engine_;

	Script_context* console_context_;

	Script_context* main_context_;
};

}
