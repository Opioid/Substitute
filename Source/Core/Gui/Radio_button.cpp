#include "Radio_button.hpp"
#include "Gui_input.hpp"
#include "../Application/Cursor.hpp"
#include "../Rendering/Printer.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Color.hpp"
#include "../Controls/Input_state.hpp"

namespace gui
{

Radio_button::Radio_button() : Component(), m_checked(false)
{}

void Radio_button::update(const Gui_input& input)
{
	const float2& pos = input.cursor.get_coord();

	float2 apos = get_absolute_position();

	if (input.mouse.is_key_click(0)
	&&  pos.x >= apos.x     && pos.x <= apos.x + 14.f
	&&  pos.y >= apos.y + 2.f && pos.y <= apos.y + 16.f)
	{
		check();
	}
		
}

void Radio_button::render(rendering::Printer& printer)
{
	printer.set_texture(s_texture);
	printer.set_color(rendering::Color4(1.f, 1.f, 1.f, 0.93f));

	float offset = m_checked ? 0.f : 13.f / 64.f;

	printer.set_position(get_absolute_position() + float2(0.f, 2.f));
	printer.set_texture_coordinates(float2(offset + 27.f / 64.f, 32.0f / 64.0f), float2(offset + 41.f / 64.f, 46.f / 64.f));
	printer.draw_quad(float2(14.f, 14.f));

	printer.flush(false);

	printer.set_font("segoeui", 13);

	printer.set_color(rendering::color4::black);
	printer.set_position(get_absolute_position() + float2(18.f, 0.f));
	printer.print(text_);

	printer.flush();
}

void Radio_button::set_text(const std::string& text)
{
	text_ = text;
}

void Radio_button::check()
{
	if (m_checked)
	{
		return;
	}

	m_checked = true;

	m_onCheck.raise(this);
}

}
