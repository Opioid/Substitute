#include "Message_sender.hpp"
#include "Message_server.hpp"


namespace logging
{
	Message_sender::Message_sender(Message_server& server) : m_server(server)
	{}


	void Message_sender::send(const std::string& text, Message::Type::Value type) const
	{
		m_server.send(Message(text, type, this));
	}


	void Message_sender::send(Message const& message) const
	{
		m_server.send(message);
	}
}