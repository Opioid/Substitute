#include "Gui_input.hpp"

namespace gui
{

Gui_input::Gui_input(const Input_state& keyboard, const Input_state& mouse, const Controls::Signal_stream& signal_stream, const Cursor& cursor, const float2& screen_size)
	: keyboard(keyboard), mouse(mouse), signal_stream(signal_stream), cursor(cursor), screen_size(screen_size)
{}

}
