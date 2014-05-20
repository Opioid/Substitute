#pragma once


#include "Message_server.hpp"
#include <string>


class Application;


namespace logging
{

	bool init(const Application& application);
	void release();

	void input(const std::string& text);
	void output(const std::string& text);
	void post(const std::string& text);
	void warning(const std::string& text);
	void error(const std::string& text);

	extern Message_server server;
}
