#pragma once


#include "Message.hpp"


namespace logging
{
	class Message_server;

	class Message_sender
	{

	public:

		Message_sender(Message_server& server);

		void send(const std::string& text, Message::Type::Value type) const;
		void send(Message const& message) const;

	private:

		Message_server& m_server;
	};
}