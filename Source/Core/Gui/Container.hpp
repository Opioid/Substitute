#pragma once

#include "gui_Component.hpp"
#include <list>

namespace gui
{

class Container : public Component
{

public:

	Container();

	void add(Component* component);

	virtual void update(const float speed);

	virtual void update(const Gui_input& input);

	virtual void render(rendering::Printer& printer);

private:

	std::list<Component*> m_components;

};

}
