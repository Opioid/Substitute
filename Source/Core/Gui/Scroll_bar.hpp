#pragma once

#include "gui_Component.hpp"

namespace gui
{

class Scroll_bar : public Component
{

public:

	Scroll_bar();

	virtual void update(const float speed);

	virtual void update(const Gui_input& input);

	virtual void render(rendering::Printer& printer);

	void setClientWindow(size_t clientWindow);
	void setClientTotal(size_t clientTotal);
	void setClientStart(size_t clientStart);

	size_t getClientStart() const;

private:

	void scroll(int d);

	size_t m_clientWindow;
	size_t m_clientTotal;
	size_t m_clientStart;

	float        m_coolDown;
	unsigned int m_lastKey;

	bool m_scrollUpSelected;
	bool m_scrollDownSelected;

	bool m_scrollButtonSelected;
	int m_scrollButtonTop;
	int m_scrollButtonHeight;
};

}
