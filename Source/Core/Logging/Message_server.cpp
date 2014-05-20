#include "Message_server.hpp"
#include "Message_receiver.hpp"
#include "Message.hpp"

namespace logging
{
	void Message_server::add_receiver(Message_receiver* receiver)
	{
		m_receivers.push_back(receiver);
	}


	void Message_server::send(const Message& message) const
	{
		for (auto i = m_receivers.begin(); i != m_receivers.end(); ++i)
		{
			if (*i != message.get_source())
			{
				(*i)->on_receive(message);
			}
		}
	}
}
