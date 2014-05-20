#include "Container.hpp"

namespace gui
{

Container::Container()
{
	set_focus(true);
}

void Container::add(Component *component)
{
	component->set_parent(this);
	m_components.push_back(component);
}

void Container::update(const float speed)
{
	for (std::list<Component*>::iterator c = m_components.begin(); c != m_components.end(); ++c)
	{
		(*c)->update(speed);
	}
}

void Container::update(const Gui_input& input)
{
	for (std::list<Component*>::iterator c = m_components.begin(); c != m_components.end(); ++c)
	{
		(*c)->update(input);
	}
}

void Container::render(rendering::Printer& printer)
{
	for (std::list<Component*>::iterator c = m_components.begin(); c != m_components.end(); ++c)
	{
		(*c)->render(printer);
	}
}

}
