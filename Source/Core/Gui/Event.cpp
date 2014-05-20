#include "Event.hpp"



namespace gui
{
	void Event::raise(Component* source) const
	{
		for (auto e = m_callbacks.begin(); e != m_callbacks.end(); ++e)
		{
			(*e)(source);
		}
	}

}