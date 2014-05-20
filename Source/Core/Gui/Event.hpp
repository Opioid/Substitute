#pragma once


#include <list>
#include <functional>


namespace gui
{

	class Component;

	class Event
	{
		
	public:

		template<typename T>
		void add(void (T::*mp)(Component*), T* callee)
		{
			m_callbacks.push_back(std::bind(mp, callee, std::placeholders::_1));
		}

		void raise(Component* source) const;

	private:

		std::list<std::function<void(Component*)>> m_callbacks;
	};


}
