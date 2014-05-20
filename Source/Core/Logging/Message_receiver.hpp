#pragma once 

namespace logging
{
	class Message;

	class Message_receiver
	{

	public:

		virtual void on_receive(const Message& message) = 0;
	};
}
