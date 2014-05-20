#include "Radio_button_set.hpp"
#include "Gui_input.hpp"
#include "Radio_button.hpp"
#include "../Rendering/Printer.hpp"
#include "Rendering/Color.hpp"

namespace gui
{
	
void Radio_button_set::update(const Gui_input& input)
{
	for (size_t i = 0; i < m_buttons.size(); ++i)
	{
		m_buttons[i].update(input);
	}
}

void Radio_button_set::render(rendering::Printer& printer)
{
	printer.set_color(rendering::Color4(0.175f, 0.175f, 0.175f, 1.f));
	printer.set_font("segoeui", 12);
	printer.set_position(get_absolute_position());
	printer.print(m_caption);
	printer.flush();
	

	for (size_t i = 0; i < m_buttons.size(); ++i)
	{
		m_buttons[i].render(printer);
	}
}

void Radio_button_set::set_caption(const std::string& caption)
{
	m_caption = caption;
}

void Radio_button_set::allocate(size_t numButtons)
{
	m_buttons.resize(numButtons);

	if (m_buttons.size())
	{
		m_buttons[0].check();
		m_checkedRadioButton = 0;
	}

	for (size_t i = 0; i < m_buttons.size(); ++i)
	{
		m_buttons[i].set_parent(this);
		m_buttons[i].set_position(float2(2.f, 16.f + float(i) * 18.f));

		m_buttons[i].m_onCheck.add<Radio_button_set>(&Radio_button_set::onRadioButtonCheck, this);
	}
}

Radio_button *Radio_button_set::getRadioButton(size_t index)
{
	if (index >= m_buttons.size())
	{
		return 0;
	}

	return &m_buttons[index];
}

size_t Radio_button_set::getCheckedRadioButton() const
{
	return m_checkedRadioButton;
}

void Radio_button_set::onRadioButtonCheck(Component *source)
{
	for (size_t i = 0; i < m_buttons.size(); ++i)
	{
		if (source == &m_buttons[i])
		{
			m_checkedRadioButton = i;
		}
		else
		{
			m_buttons[i].m_checked = false;
		}
	}
}

}
