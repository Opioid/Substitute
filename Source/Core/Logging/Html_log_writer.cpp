#include "Html_log_writer.hpp"
#include "Message.hpp"
#include "Application/Application.hpp"
#include "Timing/Time.hpp"
#include <boost/algorithm/string.hpp> 

namespace logging
{

Html_log_writer::~Html_log_writer()
{
	stream_ << "<h2>Closing Log @ " << timing::date_string() << " " << timing::time_string() << "</h2>" << std::endl;

	stream_ << "</body></html>";
}

bool Html_log_writer::start(const Application& app)
{
	stream_.open("../log.html");

	if (!stream_)
	{
		return false;
	}

	stream_ << "<html><head><style>" << std::endl;

	stream_ << "body {background:rgb(24, 24, 24); font-size:1.125em; font-family:\"Consolas\", Helvetica, sans-serif}" << std::endl;
	stream_ << "h1 {font-size:1.5em; font-family:\"Segoe UI\", Helvetica, sans-serif; font-style:italic; color:rgb(127,127,127);}" << std::endl;
	stream_ << "h2 {font-size:1.25em; color:rgb(127,127,127);}" << std::endl;
	stream_ << "span.input {color:white;}" << std::endl;
	stream_ << "span.output {color:rgb(143,143,255);}" << std::endl;
	stream_ << "span.ok {color:rgb(64,255,64);}" << std::endl;
	stream_ << "span.warning {color:orange;}" << std::endl;
	stream_ << "span.error {color:red;}" << std::endl;

	stream_ << "</style></head><body>" << std::endl;

	stream_ << "<h1>" << app.name() << " - " << app.build_info() << "</h1>" << std::endl;
	stream_ << "<h2>Opening Log @ " << timing::date_string() << " " << timing::time_string() << "</h2>" << std::endl;

	return true;
}

void Html_log_writer::on_receive(const Message& message)
{
	switch (message.type())
	{
		case Message::Type::Input:   stream_ << "<span class=\"input\">";   break;
		case Message::Type::Output:  stream_ << "<span class=\"output\">";  break;
		case Message::Type::Ok:      stream_ << "<span class=\"ok\">";      break;
		case Message::Type::Warning: stream_ << "<span class=\"warning\">"; break;
		case Message::Type::Error:   stream_ << "<span class=\"error\">";   break;
	}

	std::string text = message.text();
	boost::replace_all(text, "<", "&lt;");
	boost::replace_all(text, ">", "&gt;");

	stream_ << text;

	stream_ << "</span>";

	stream_ << "<br/>" << std::endl;
}

}
