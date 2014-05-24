#include "Message_sender.hpp"
#include "Message_server.hpp"

namespace logging
{

Message_sender::Message_sender(Message_server& server) : server_(server)
{}

void Message_sender::send(const std::string& text, Message::Type type) const
{
	server_.send(Message(text, type, this));
}

void Message_sender::send(const Message& message) const
{
	server_.send(message);
}

}
