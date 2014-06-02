#include "Logging.hpp"
#include "Message.hpp"
#include "Html_log_writer.hpp"

namespace logging
{

Html_log_writer log_writer;

bool init(const Application& application)
{
	log_writer.start(application);

	server.add_receiver(&log_writer);

	return true;
}

void release()
{}

void input(const std::string& text)
{
	server.send(Message(text, Message::Type::Input));
}

void output(const std::string& text)
{
	server.send(Message(text, Message::Type::Output));
}

void post(const std::string& text)
{
	server.send(Message(text, Message::Type::Ok));
}

void warning(const std::string& text)
{
	server.send(Message(text, Message::Type::Warning));
}

void error(const std::string& text)
{
	server.send(Message(text, Message::Type::Error));
}

Message_server server;

}
