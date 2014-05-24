#include "Input_line.hpp"
#include "Gui_input.hpp"
#include "../Application/Cursor.hpp"
#include "../Rendering/Printer.hpp"
#include "Rendering/Color.hpp"
#include "Rendering/Resource_view.hpp"
#include "../Controls/Input_state.hpp"
#include "String/String.hpp"
#include "Platform/Input_signal.hpp"

namespace gui
{

Input_line::Marked_region::Marked_region() : position(0), length(0), started_position(0), stopped_position(0), marking(false)
{}

bool Input_line::Marked_region::is_marked() const
{
	return length > 0;
}

void Input_line::Marked_region::reset()
{
	position = 0;
	length = 0;

	started_position = 0;
	stopped_position = 0;

	marking = false;
}

Input_line::Input_line() : cursor_position_(0), m_maxHistory(8), m_character_width(10), m_cursor_blink(0.f)
{}

void Input_line::update(const float speed)
{
	m_cursor_blink -= speed;

	if (m_cursor_blink < 0.f)
	{
		m_cursor_blink = 1.f;
	}
}
	
void Input_line::update(const Gui_input& input)
{
	for (size_t i = 0; i < input.signal_stream.size(); ++i)
	{
		process_signal(input.signal_stream[i]);
	}

	const float2& pos = input.cursor.coord();

	float2 apos = get_absolute_position();

	if (input.mouse.is_key_click(0)
	&& pos.x >= apos.x && pos.x <= apos.x + size_.x
	&& pos.y >= apos.y && pos.y <= apos.y + size_.y)
	{
		marked_.reset();
		marked_.marking = true;
		marked_.started_position = (pos.x - apos.x - 2) / m_character_width;
	}

	if (marked_.marking && input.mouse.is_key_down(0))
	{
		marked_.stopped_position = (pos.x - apos.x - 2) / m_character_width;

		move_to((pos.x - apos.x - 2) / m_character_width);
	}
	else
	{
		marked_.marking = false;
	}

	if (marked_.marking)
	{
		resolve_marked_region();
	}
}

void Input_line::render(rendering::Printer& printer)
{
	printer.set_texture(nullptr);
	printer.set_color(rendering::Color4(0.0075f, 0.0075f, 0.0075f, 0.975f));

	printer.set_position(get_absolute_position());
	printer.draw_quad(size_);

	if (marked_.length > 0)
	{
		printer.set_color(rendering::Color4(0.1f, 0.1f, 0.12f));
		printer.set_position(get_absolute_position() + float2(2.f + m_character_width * marked_.position, 0.f));
		printer.draw_quad(float2(m_character_width * marked_.length, size_.y));
	}

	printer.flush(false);

	printer.set_font("Consola", 18);

	printer.set_color(rendering::color4::white);
	printer.set_position(get_absolute_position() + float2(2.f, 0.f));
	printer.print(text_);

	if (has_focus())
	{
		printer.set_position(get_absolute_position() + float2(2.f + m_character_width * cursor_position_, 0.f));
		printer.set_color(rendering::Color4(1.f, 1.f, 1.f, m_cursor_blink));
		printer.print("_");
	}

	printer.flush();
}

const std::string& Input_line::get_text() const
{
	return text_;
}

void Input_line::set_text(const std::string& text)
{
	text_ = text;
	cursor_position_ = int(text_.length());
	marked_.reset();
}

void Input_line::insert(int pos, const std::string& text)
{
	text_.insert(pos, text);
	cursor_position_ = pos + int(text.length());
}

void Input_line::erase(int pos, int length)
{
	if (!text_.length())
	{
		return;
	}

	if (pos < 0)
	{
		pos = 0;
	}

	text_.erase(pos, length);
}

void Input_line::move(int delta)
{
	move_to(cursor_position_ + delta);
}

void Input_line::move_to(int pos)
{
	cursor_position_ = pos;

	if (cursor_position_ < 0)
	{
		cursor_position_ = 0;
	}
	else if (cursor_position_ > int(text_.length()))
	{
		cursor_position_ = int(text_.length());
	}
}

void Input_line::add_to_history(const std::string& text)
{
	if (m_history.size() >= m_maxHistory)
	{
		m_history.pop_back();
	}

	m_history.push_front(text);

	m_history_pos = 0;
}

void Input_line::scroll(int delta)
{
	if (m_history_pos == 0)
	{
		m_current_line = text_;
	}

	m_history_pos += delta;

	if (m_history_pos < 0)
	{
		m_history_pos = 0;
	}

	if (m_history_pos > int(m_history.size()))
	{
		m_history_pos = int(m_history.size());
	}

	if (m_history_pos == 0)
	{
		set_text(m_current_line);
	}
	else
	{
		auto h = m_history.begin();
		std::advance(h, m_history_pos - 1);
		set_text(*h);
	}
}

void Input_line::process_signal(const platform::Input_signal& signal)
{
	/*
	switch (key)
	{
	case VK_LEFT:
			move(-1);
			return;
		case VK_RIGHT:
			move(1); 
			return;
		case VK_BACK:
			{
				if (marked_.is_marked())
				{
					erase_marked_region();
				}
				else
				{
					erase(cursor_position_ - 1, 1);
					move(-1);
				}

				return;
			}
		case VK_UP:
			scroll(1);
			return;
		case VK_DOWN:
			scroll(-1);
			return;
		case VK_DELETE: 
			{
				if (marked_.is_marked())
				{
					erase_marked_region();
				}
				else				  
				{
					erase(cursor_position_, 1);
				}

				return;
			}
		}

		if (is_banned_key(key))
		{
			return;
		}

		if (marked_.is_marked())
		{
			erase_marked_region();
		}

	std::string tmp;
	tmp = string::vkToAscii(key);
	*/

	if (signal.is_character())
	{
		if (marked_.is_marked())
        {
            erase_marked_region();
        }

		std::string character;
		character = char(signal.id());

		insert(cursor_position_, character);
	}
	else if (signal.is_pressed())
	{
		uint32_t key = signal.id();

		if (platform::key::Enter == key)
		{
			if (text_ != "")
			{
				add_to_history(text_);
				m_on_accept.raise(this);
			}
		}
		else if (platform::key::Left == key)
		{
			move(-1);
		}
		else if (platform::key::Right == key)
		{
			move(1);
		}
		else if (platform::key::Up == key)
		{
			scroll(1);
		}
		else if (platform::key::Down == key)
		{
			scroll(-1);
		}
		else if (platform::key::Backspace == key)
		{
			if (marked_.is_marked())
			{
				erase_marked_region();
			}
			else
			{
				erase(cursor_position_ - 1, 1);
				move(-1);
			}
		}
		else if (platform::key::Delete == key)
		{
			if (marked_.is_marked())
			{
				erase_marked_region();
			}
			else
			{
				erase(cursor_position_, 1);
			}
		}
	}
}

void Input_line::erase_marked_region()
{
	erase(marked_.position, marked_.length);
	move_to(marked_.position);
	marked_.reset();
}

void Input_line::resolve_marked_region()
{
	if (marked_.started_position < 0)
	{
		marked_.started_position = 0;
	}
	if (marked_.stopped_position < 0)
	{
		marked_.stopped_position = 0;
	}

	if (marked_.started_position < marked_.stopped_position)
	{
		marked_.position = marked_.started_position;
		marked_.length = marked_.stopped_position - marked_.position;
	}
	else
	{
		marked_.position = marked_.stopped_position;
		marked_.length = marked_.started_position - marked_.position;
	}

	if (marked_.position < 0)
	{
		marked_.position = 0;
	}

	if (marked_.position > int(text_.length()))
	{
		marked_.position = int(text_.length());
		marked_.length = 0;
	}

	if (marked_.position + marked_.length > int(text_.length()))
	{
		marked_.length = int(text_.length()) - marked_.position;
	}
}

bool Input_line::is_banned_key(uint32_t key)
{
	if (platform::key::Grave_accent == key
	||  platform::key::Escape == key
	||  platform::key::Enter == key
	||  platform::key::F1 == key
	||  platform::key::F2 == key
	||  platform::key::F3 == key
	||  platform::key::F4 == key
	||  platform::key::F5 == key
	||  platform::key::F6 == key
	||  platform::key::F7 == key
	||  platform::key::F8 == key
	||  platform::key::F9 == key
	||  platform::key::F10 == key
	||  platform::key::F11 == key
	||  platform::key::F12 == key
	||  platform::key::Left_shift == key
	||  platform::key::Left_control == key
	||  platform::key::Right_shift == key
	||  platform::key::Right_control == key
//	case VK_CAPITAL == key
	||  platform::key::Left == key
	||  platform::key::Right == key
	||  platform::key::Up == key
	||  platform::key::Down == key
//	case VK_BACK:
//	case VK_DELETE:
//	case VK_TAB:
//	case VK_LWIN:
//	case VK_RWIN:
//	case VK_MENU:
//	case VK_PRIOR:
//	case VK_NEXT:
	||  platform::key::Left_control == key)
	{
		return true;
	}

	return false;
}

}
