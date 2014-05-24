#pragma once

#include "Math/Vector.hpp"
#include <string>
#include <stack>

struct SDL_Window;

namespace platform
{

enum class Message
{

	Nothing,
	Something,
	Quit

};

class Client;
class Input_signal;

class Input_sink
{

public:

	virtual ~Input_sink()
	{}

protected:

	virtual void on_input_signal(const Input_signal& signal) = 0;

	friend Client;
};

class Client
{

public:

	Client();
	~Client();

	bool create(const std::string& title, const uint2& dimensions, Input_sink& input_sink);

	SDL_Window* handle() const;

    void update();

    Message query_message();

	const uint2& dimensions() const;

    void show_system_cursor(bool show);

	float2 relative_cursor_position() const;

	void set_title(const std::string& title);

private:

	void on_input_signal(const Input_signal& signal);

	bool valid_;

	SDL_Window* window_;

	uint2 dimensions_;

	Input_sink* input_sink_;

	std::stack<Message> messages_;
};

}
