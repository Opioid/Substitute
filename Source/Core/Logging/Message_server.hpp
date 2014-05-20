#pragma once

#include <list>

namespace logging
{
	class Message;
	class Message_receiver;

	class Message_server
	{

	public:

		void add_receiver(Message_receiver* receiver);

		void send(const Message& message) const;

	private:

		std::list<Message_receiver*> m_receivers;
	};
}
