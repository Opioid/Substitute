#include "Console.hpp"
#include "../Rendering/Printer.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Color.hpp"
#include "../Scripting/Script_tool.hpp"
#include "../Logging/Message_server.hpp"


namespace gui
{

Console::Console(logging::Message_server &server, scripting::Script_tool& script_tool)
    : Frame(false), logging::Message_sender(server), script_tool_(script_tool)
{
	set_caption("Console");

	server.add_receiver(&m_message_viewer);
	m_message_viewer.set_position(float2(0.f, 0.f));
	m_container.add(&m_message_viewer);

	m_input_line.set_size(float2(0.f, 26.f));
	m_input_line.set_focus(true);
	m_container.add(&m_input_line);

	m_input_line.m_on_accept.add<Console>(&Console::on_input_line_accept, this);

	m_close_button.set_enabled(false);
}


Message_viewer& Console::query_message_viewer()
{
	return m_message_viewer;
}

void Console::on_input_line_accept(Component* /*source*/)
{
	send(m_input_line.get_text(), logging::Message::Type::Input);

	script_tool_.engine().execute_string(m_input_line.get_text());

	m_input_line.set_text("");
}

void Console::on_set_size()
{
	Frame::on_set_size();

	m_message_viewer.set_size(get_client_size() - float2(0.f, 4.f + m_input_line.dimensions().y));

	m_input_line.set_position(float2(0.f, m_message_viewer.dimensions().y + 4.f));
	m_input_line.set_size(float2(get_client_size().x, 26.f));
}

void Console::render_private(rendering::Printer& printer)
{
	printer.set_texture(nullptr);
	printer.set_color(rendering::Color4(0.02f, 0.02f, 0.025f));

	// thing between messageviewer and inputline
	printer.set_position(get_absolute_position() + get_client_offset() + float2(0.f, m_message_viewer.dimensions().y));
	printer.draw_quad(float2(get_client_size().x, 4.f));

	printer.flush(false);
}

}
