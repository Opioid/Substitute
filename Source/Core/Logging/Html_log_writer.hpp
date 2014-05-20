#pragma once

#include "Message_receiver.hpp"
#include <fstream>

class Application;

namespace logging
{
	class Html_log_writer : public Message_receiver
	{

	public:

		~Html_log_writer();

		bool start(const Application& app);

		void on_receive(const Message& message);

	private:

		std::ofstream m_stream;
	};
}
