#pragma once

#include "Frame.hpp"
#include "Message_viewer.hpp"
#include "Input_line.hpp"
#include "Logging/Message_sender.hpp"

namespace scripting
{

class Script_tool;

}

namespace gui
{

class Console : public Frame, logging::Message_sender
{

public:

	Console(logging::Message_server &server, scripting::Script_tool& script_tool);

	Message_viewer& message_viewer();

	void on_input_line_accept(Component *source);

private:

	virtual void on_set_size();

	void render_private(rendering::Printer& printer);

	Message_viewer message_viewer_;
	Input_line     input_line_;

	scripting::Script_tool& script_tool_;
};

}
