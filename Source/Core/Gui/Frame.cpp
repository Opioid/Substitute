#include "Frame.hpp"
#include "Gui_input.hpp"
#include "../Application/Cursor.hpp"
#include "Frame_manager.hpp"
#include "../Rendering/Printer.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Color.hpp"
#include "../Controls/Input_state.hpp"

namespace gui
{

const float Frame::s_top_border_height = 40.f;
const float Frame::s_bottom_border_height = 4.f;
const float Frame::s_side_boder_width = 4.f;

const rendering::Color4 Frame::s_frame_color       = rendering::Color4(0.004f,  0.004f,  0.008f, 1.f);
const rendering::Color4 Frame::s_frame_color_focus = rendering::Color4(0.0075f, 0.0075f, 0.015f, 1.f);

Frame::Frame(bool drawBackground)
    : Component(), m_minimize_button(Border_button::Type::Minimize), m_close_button(Border_button::Type::Close),
      m_manager(nullptr), m_current_state(State::Windowed), m_drawBackground(drawBackground)
{
	m_minimize_button.set_parent(this);
	m_minimize_button.m_on_click.add<Frame>(&Frame::on_minimize_button_clicked, this);

	m_close_button.set_parent(this);
	m_close_button.m_on_click.add<Frame>(&Frame::on_close_button_clicked, this);

	m_container.set_parent(this);
	m_container.set_position(float2(s_side_boder_width, s_top_border_height));
}

void Frame::setManager(Frame_manager* manager)
{
	m_manager = manager;
}

void Frame::update(const float speed)
{
	m_container.update(speed);
}

void Frame::update(const Gui_input& input)
{
	const float2& pos = input.cursor.get_coord();
	const float2& oldPos = input.cursor.get_previous_coord();

	float2 apos = get_absolute_position();

	if (input.mouse.is_key_click(0)
	&&  pos.x >= apos.x && pos.x <= apos.x + size_.x
	&&  pos.y >= apos.y && pos.y <= apos.y + s_top_border_height)
	{
		m_grabbed = true;
	}
	else if (!input.mouse.is_key_down(0))
	{
		m_grabbed = false;
	}

	if (m_grabbed && input.mouse.is_key_down(0))
	{
		float2 new_position = apos + pos - oldPos;

		if (new_position.x < -size_.x + 30.f)
		{
			new_position.x = -size_.x + 30.f;
		}

		if (new_position.y < -10)
		{
			new_position.y = -10;
		}

		set_position(new_position);
	}

	m_minimize_button.update(input);
	m_close_button.update(input);

	m_container.update(input);
}

void Frame::render(rendering::Printer& printer)
{
	if (m_drawBackground)
	{
		printer.set_texture(nullptr);
		printer.set_color(rendering::Color4(0.88f, 0.88f, 0.88f, 0.825f));

		printer.set_position(get_absolute_position() + float2(s_side_boder_width, s_top_border_height));
		printer.draw_quad(size_ - float2(2.f * s_side_boder_width, s_top_border_height + s_bottom_border_height));
			
		printer.flush(false);
	}

	printer.set_texture(nullptr);

	if (has_focus())
	{
		printer.set_color(s_frame_color_focus);
	}
	else
	{
		printer.set_color(s_frame_color);
	}

	// Top
	printer.set_position(position);
	printer.draw_quad(float2(size_.x, s_top_border_height));

	// Left
	printer.set_position(position + float2(0.f, s_top_border_height));
	printer.draw_quad(float2(s_side_boder_width, size_.y - s_top_border_height - s_bottom_border_height));

	// Right
	printer.set_position(position + float2(size_.x - s_side_boder_width, s_top_border_height));
	printer.draw_quad(float2(s_side_boder_width, size_.y - s_top_border_height - s_bottom_border_height));

	// Bottom
	printer.set_position(position + float2(0.f, size_.y - s_bottom_border_height));
	printer.draw_quad(float2(size_.x, s_bottom_border_height));

	printer.flush(false);
	printer.set_texture(s_texture);

	m_minimize_button.render(printer);
	m_close_button.render(printer);

	printer.flush(false);

	printer.set_color(rendering::Color4(0.f, 0.3f, 0.6f));
	printer.set_font("segoeui", 28);
	printer.set_position(position + float2(18.f, 3.f));
	printer.print(m_caption);

	printer.flush();

	render_private(printer);

	m_container.render(printer);
}

const std::string& Frame::get_caption() const
{
	return m_caption;
}

void Frame::set_caption(const std::string& caption)
{
	m_caption = caption;
}

float2 Frame::get_client_size() const
{
	return float2(size_.x - 2.f * s_side_boder_width, size_.y - s_top_border_height - s_bottom_border_height);
}

void Frame::set_client_size(const float2& size)
{
	set_size(float2(size.x + 2.f * s_side_boder_width, size.y + s_top_border_height + s_bottom_border_height));
}

float2 Frame::get_client_offset() const
{
	return float2(s_side_boder_width, s_top_border_height);
}

bool Frame::is_minimized() const
{
	return State::Minimized == m_current_state;
}

bool Frame::is_close_pending() const
{
	return State::Close_pending == m_current_state;
}

void Frame::minimize()
{
	set_state(State::Minimized);
}

void Frame::restore()
{
	set_state(State::Windowed);
}

void Frame::close()
{
	set_state(State::Close_pending);
}

void Frame::on_set_size()
{
	m_minimize_button.set_position(float2(size_.x - (m_minimize_button.get_size().x + m_close_button.get_size().x + 3.f * s_side_boder_width), 4.f));
	m_close_button.set_position(float2(size_.x - (m_close_button.get_size().x + s_side_boder_width), 4.f));
}

void Frame::set_state(State state)
{
	m_current_state = state;

	if (m_manager)
	{
		m_manager->frame_change_state(this);
	}
}

void Frame::on_minimize_button_clicked(Component* /*source*/)
{
	minimize();
}

void Frame::on_close_button_clicked(Component* /*source*/)
{
	close();
}

void Frame::render_private(rendering::Printer& /*printer*/)
{}

}
