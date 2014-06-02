#include "Console.hpp"
#include "Rendering/Printer.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Color.hpp"
#include "Scripting/Script_tool.hpp"
#include "Math/Vector.inl"
#include "Logging/Message_server.hpp"

namespace gui
{

Console::Console(logging::Message_server &server, scripting::Script_tool& script_tool) : Frame(false), logging::Message_sender(server), script_tool_(script_tool)
{
	set_caption("Console");

	server.add_receiver(&message_viewer_);
	message_viewer_.set_position(float2(0.f, 0.f));
	m_container.add(&message_viewer_);

	input_line_.set_size(float2(0.f, 26.f));
	input_line_.set_focus(true);
	m_container.add(&input_line_);

	input_line_.m_on_accept.add<Console>(&Console::on_input_line_accept, this);

	m_close_button.set_enabled(false);
}


Message_viewer& Console::message_viewer()
{
	return message_viewer_;
}

void Console::on_input_line_accept(Component* /*source*/)
{
	send(input_line_.text(), logging::Message::Type::Input);

	script_tool_.execute_string(input_line_.text());

	input_line_.set_text("");
}

void Console::on_set_size()
{
	Frame::on_set_size();

	message_viewer_.set_size(get_client_size() - float2(0.f, 4.f + input_line_.dimensions().y));

	input_line_.set_position(float2(0.f, message_viewer_.dimensions().y + 4.f));
	input_line_.set_size(float2(get_client_size().x, 26.f));
}

void Console::render_private(rendering::Printer& printer)
{
	printer.set_texture(nullptr);
	printer.set_color(rendering::Color4(0.02f, 0.02f, 0.025f));

	// thing between messageviewer and inputline
	printer.set_position(get_absolute_position() + get_client_offset() + float2(0.f, message_viewer_.dimensions().y));
	printer.draw_quad(float2(get_client_size().x, 4.f));

	printer.flush(false);
}

}
