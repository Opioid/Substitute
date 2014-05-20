#pragma once

#include "gui_Component.hpp"
#include "Event.hpp"

namespace gui
{

class Button : public Component
{

public:

	Button();

	virtual void update(const Gui_input& input);

	bool is_hovered_over() const;

	Event m_on_click;

protected:

	bool m_hovered_over;

private:

};

}
