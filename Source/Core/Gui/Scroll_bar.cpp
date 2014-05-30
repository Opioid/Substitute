#include "Scroll_bar.hpp"
#include "Gui_input.hpp"
#include "Application/Cursor.hpp"
#include "Rendering/Printer.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Color.hpp"
#include "Controls/Input_state.hpp"
#include "Math/Vector.inl"

namespace gui
{

Scroll_bar::Scroll_bar() : m_coolDown(0.0f), m_lastKey(0), m_scrollUpSelected(false), m_scrollDownSelected(false)
{}


void Scroll_bar::update(const float speed)
{
	if (m_coolDown > 0.f)
	{
		m_coolDown -= speed;
	}
}

void Scroll_bar::update(const Gui_input& input)
{
	const float2& pos = input.cursor.coord();
	const float2& oldPos = input.cursor.previous_coord();

	float2 apos = get_absolute_position();


	m_scrollUpSelected = pos.x >= apos.x && pos.x <= apos.x + 14.f
		          && pos.y >= apos.y && pos.y <= apos.y + 14.f;

	m_scrollDownSelected = pos.x >= apos.x && pos.x <= apos.x + 14.f
		            && pos.y >= apos.y + size_.y - 14 && pos.y <= apos.y + size_.y;

	if (input.mouse.is_key_down(0) && m_scrollUpSelected && !m_scrollButtonSelected)
	{
		if (m_coolDown <= 0.f || m_lastKey != 1000)
		{
			scroll(-1);
			m_lastKey = 1000;
			m_coolDown = 0.05f;
		}
	}

	if (input.mouse.is_key_down(0) && m_scrollDownSelected && !m_scrollButtonSelected)
	{
		if (m_coolDown <= 0.f || m_lastKey != 1001)
		{
			scroll(1);
			m_lastKey = 1001;
			m_coolDown = 0.05f;
		}
	}

/*
		int mouseWheel = mouse.getAxis(2);

		if (mouseWheel) scroll(mouseWheel > 0 ? -1 : 1);


		if (keyboard.isKeyDown(VK_PRIOR))
		{
			if (m_coolDown <= 0.0f || m_lastKey != VK_PRIOR)
			{
				scroll(-1);
				m_lastKey = VK_PRIOR;
				m_coolDown = 0.05f;
			}
		}


		if (keyboard.isKeyDown(VK_NEXT))
		{
			if (m_coolDown <= 0.f || m_lastKey != VK_NEXT)
			{
				scroll(1);
				m_lastKey = VK_NEXT;
				m_coolDown = 0.05f;
			}
		}
		*/


	if (input.mouse.is_key_click(0)
	&&  pos.x >= apos.x && pos.x <= apos.x + 14
	&&  pos.y >= apos.y + 14 + m_scrollButtonTop&& pos.y <= apos.y + 14 + m_scrollButtonTop + m_scrollButtonHeight)
	{
		m_scrollButtonSelected = true;
	}
	else if (!input.mouse.is_key_down(0))
	{
			m_scrollButtonSelected = false;
	}

	if (m_scrollButtonSelected && input.mouse.is_key_down(0))
	{
		int newTop = m_scrollButtonTop + pos.y - oldPos.y;

		if (newTop < 0)
		{
			newTop = 0;
		}
		else if (newTop > size_.y - 28 - m_scrollButtonHeight)
		{
			newTop = size_.y - 28 - m_scrollButtonHeight;
		}

		m_scrollButtonTop = newTop;

		m_clientStart = int(float(m_scrollButtonTop) / float(size_.y - 28) * float(m_clientTotal) + 0.5f);
	}
}

void Scroll_bar::render(rendering::Printer& printer)
{
	printer.set_texture(s_texture);
	printer.set_color(rendering::color4::white);

	// upper arrow
	float offset = m_scrollUpSelected ? 14.f / 64.f : 0.f;
	printer.set_position(get_absolute_position());
	printer.set_texture_coordinates(float2(0.f, offset + 32.f / 64.f), float2(14.f / 64.f, offset + 46.f / 64.f));
	printer.draw_quad(float2(14, 14));

	// lower arrow
	offset = m_scrollDownSelected ? 14.f / 64.f : 0.f;
	printer.set_position(get_absolute_position() + float2(0, size_.y - 14));
	printer.set_texture_coordinates(float2(0.f, offset + 45.75f / 64.f), float2(14.f / 64.f, offset + 31.75f / 64.f));
	printer.draw_quad(float2(14, 14));

	// bar
	printer.set_position(get_absolute_position() + float2(0, 14));
	printer.set_texture_coordinates(float2(14.f / 64.f, 38.f / 64.f), float2(28.f / 64.f, 41.f / 64.f));
	printer.draw_quad(float2(14, size_.y - 28));

	// top
	printer.set_position(get_absolute_position() + float2(0, 14 + m_scrollButtonTop));
	printer.set_texture_coordinates(float2(14.f / 64.f, 32.f / 64.f), float2(28.f / 64.f, 34.f / 64.f));
	printer.draw_quad(float2(14, 2));

	// middle
	printer.set_position(get_absolute_position() + float2(0, 16 + m_scrollButtonTop));
	printer.set_texture_coordinates(float2(14.f / 64.f, 34.f / 64.f), float2(28.0f / 64.f, 35.75f / 64.f));
	printer.draw_quad(float2(14, m_scrollButtonHeight - 4));

	// bottom
	printer.set_position(get_absolute_position() + float2(0, 12 + m_scrollButtonTop + m_scrollButtonHeight));
	printer.set_texture_coordinates(float2(14.f / 64.f, 33.75f / 64.f), float2(28.f / 64.f, 32.f / 64.f));
	printer.draw_quad(float2(14, 2));

	printer.flush(false);
}

void Scroll_bar::setClientWindow(size_t clientWindow)
{
	m_clientWindow = clientWindow;
}

void Scroll_bar::setClientTotal(size_t clientTotal)
{
	m_clientTotal = clientTotal;

	float height = m_clientTotal <= m_clientWindow ? float(size_.y - 28) : (float(m_clientWindow) / float(m_clientTotal)) * float(size_.y - 28);
	m_scrollButtonHeight = int(height + 0.5f);
}

void Scroll_bar::setClientStart(size_t clientStart)
{
	m_clientStart = clientStart;

	m_scrollButtonTop = int(float(m_clientStart) / float(m_clientTotal) * float(size_.y - 28) + 0.5f);
}

size_t Scroll_bar::getClientStart() const
{
	return m_clientStart;
}

void Scroll_bar::scroll(int d)
{
	int clientStart = int(m_clientStart) + d;

	int m = int(m_clientTotal - m_clientWindow);

	if (clientStart > m)
	{
		clientStart = m;
	}

	if (clientStart < 0 || m_clientTotal <= m_clientWindow)
	{
		clientStart = 0;
	}

	setClientStart(size_t(clientStart));
}

}
