#pragma once

#include "gui_Component.hpp"
#include <string>
#include <vector>

namespace gui
{

class Radio_button;

class Radio_button_set : public Component
{

public:

	virtual void update(const Gui_input& input);

	virtual void render(rendering::Printer& printer);

	void allocate(size_t numButtons);

	void set_caption(const std::string& caption);

	Radio_button *getRadioButton(size_t index);

	size_t getCheckedRadioButton() const;

	void onRadioButtonCheck(Component *source);

private:

	std::string m_caption;

	std::vector<Radio_button> m_buttons;

	size_t m_checkedRadioButton;

};

}

