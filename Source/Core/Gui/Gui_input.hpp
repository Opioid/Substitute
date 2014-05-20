#pragma once

#include "../Controls/Controls.hpp"
#include <Math/Vector.hpp>

class Input_state;
class Cursor;

namespace gui
{

struct Gui_input
{
    Gui_input(const Input_state& keyboard, const Input_state& mouse, const Controls::Signal_stream& signal_stream, const Cursor& cursor, const float2& screen_size);

	const Input_state& keyboard;
	const Input_state& mouse;

	const Controls::Signal_stream& signal_stream;

	const Cursor& cursor;

    const float2 screen_size;
};

}


