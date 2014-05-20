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

	bool create(const std::string& title, const uint2& size, Input_sink& input_sink);

    SDL_Window* get_handle() const;

    void update();

    Message get_message();

	const uint2& get_size() const;

    void show_system_cursor(bool show);

	float2 get_relative_cursor_position() const;

	void set_title(const std::string& title);

private:

	bool valid_;

	SDL_Window* window_;

	uint2 size_;

	Input_sink* input_sink_;

	std::stack<Message> messages_;

public:

	// Hacky Hack
	void on_input_signal(const Input_signal& signal);
	void add_message(Message message);
};
}
