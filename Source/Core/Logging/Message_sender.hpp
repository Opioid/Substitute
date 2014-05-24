#pragma once

#include "Message.hpp"

namespace logging
{

class Message_server;

class Message_sender
{

public:

	Message_sender(Message_server& server);

	void send(const std::string& text, Message::Type type) const;
	void send(const Message& message) const;

private:

	Message_server& server_;
};

}
