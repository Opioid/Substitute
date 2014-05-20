#include "Html_log_writer.hpp"
#include "Message.hpp"
#include "../Application/Application.hpp"
#include "Timing/Time.hpp"
#include <boost/algorithm/string.hpp> 

namespace logging
{
	Html_log_writer::~Html_log_writer()
	{
		m_stream << "<h2>Closing Log @ " << timing::get_date_string() << " " << timing::get_time_string() << "</h2>" << std::endl;

		m_stream << "</body></html>";
	}

	bool Html_log_writer::start(const Application& app)
	{
		m_stream.open("log.html");

		if (!m_stream) 
		{
			return false;
		}

		m_stream << "<html><head><style>" << std::endl;

		m_stream << "body {background:rgb(24, 24, 24); font-size:1.125em; font-family:\"Consolas\", Helvetica, sans-serif}" << std::endl; 
		m_stream << "h1 {font-size:1.5em; font-family:\"Segoe UI\", Helvetica, sans-serif; font-style:italic; color:rgb(127,127,127);}" << std::endl; 
		m_stream << "h2 {font-size:1.25em; color:rgb(127,127,127);}" << std::endl; 
		m_stream << "span.input {color:white;}" << std::endl;
		m_stream << "span.output {color:rgb(143,143,255);}" << std::endl;
		m_stream << "span.ok {color:rgb(64,255,64);}" << std::endl;
		m_stream << "span.warning {color:orange;}" << std::endl;
		m_stream << "span.error {color:red;}" << std::endl;

		m_stream << "</style></head><body>" << std::endl;

		m_stream << "<h1>" << app.get_name() << " - " << app.get_build_info() << "</h1>" << std::endl;
		m_stream << "<h2>Opening Log @ " << timing::get_date_string() << " " << timing::get_time_string() << "</h2>" << std::endl;

		return true;
	}

	void Html_log_writer::on_receive(const Message& message)
	{
		switch (message.get_type())
		{
			case Message::Type::Input:   m_stream << "<span class=\"input\">";   break;
			case Message::Type::Output:  m_stream << "<span class=\"output\">";  break;
			case Message::Type::Ok:      m_stream << "<span class=\"ok\">";      break;
			case Message::Type::Warning: m_stream << "<span class=\"warning\">"; break;
			case Message::Type::Error:   m_stream << "<span class=\"error\">";   break;
		}

		std::string text = message.get_text();
		boost::replace_all(text, "<", "&lt;");
		boost::replace_all(text, ">", "&gt;");

		m_stream << text;

		m_stream << "</span>";

		m_stream << "<br/>" << std::endl;
	}
}
