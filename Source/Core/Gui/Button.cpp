#include "Button.hpp"
#include "Gui_input.hpp"
#include "Gui_input.hpp"
#include "../Application/Cursor.hpp"
#include "../Controls/Input_state.hpp"

namespace gui
{

Button::Button() : m_hovered_over(false)
{}

void Button::update(const Gui_input& input)
{
	const float2& pos = input.cursor.get_coord();

	float2 apos = get_absolute_position();

	m_hovered_over = pos.x >= apos.x && pos.x <= apos.x + size_.x
		      && pos.y >= apos.y && pos.y <= apos.y + size_.y;

	if (m_enabled && m_hovered_over && input.mouse.is_key_click(0))
	{
		m_on_click.raise(this);
	}
}

bool Button::is_hovered_over() const
{
	return m_hovered_over;
}

}
