#pragma once

#include "Console.hpp"
#include "Frame_manager.hpp"

class Resource_manager;
class Input_state;
class Cursor;

namespace rendering
{

class Printer;

}

namespace logging
{

class Message_server;

}

namespace gui
{

class Frame;
class Console;
	
class Gui
{

public:

	Gui(logging::Message_server& message_server, scripting::Script_tool& Script_tool);

	bool init(Resource_manager& resource_manager, rendering::Printer& printer);

	bool is_active() const;
	bool toggle_active();

	void clear();
	void add(Frame* frame);

	void update(const float speed);
	void update(const Gui_input& input);

	void render(rendering::Printer& printer);

	void create_image_viewer(const Handle<rendering::Shader_resource_view>& texture, const std::string& image_name);

private:

	Console m_console;

	Frame_manager m_frame_manager;

    bool m_is_active;
};

}
