#pragma once

#include "gui_Component.hpp"
#include "Event.hpp"
#include <string>

namespace gui
{

class Radio_button_set;

class Radio_button : public Component
{

public:

	Event m_onCheck;

	Radio_button();

	virtual void update(const Gui_input& input);

	virtual void render(rendering::Printer& printer);

	void set_text(const std::string& text);

	void check();

private:

	std::string text_;

	bool m_checked;

	friend Radio_button_set;
};

}

