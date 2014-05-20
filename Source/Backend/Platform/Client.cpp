#include "Client.hpp"
#include "Input_signal.hpp"
#include "Platform.hpp"

namespace platform
{

Client::Client() : window_(nullptr), input_sink_(nullptr)
{
	valid_ = SDL_Init(SDL_INIT_EVERYTHING) == 0;
}

Client::~Client()
{
    SDL_Quit();
}

bool Client::create(const std::string& title, const uint2& size, Input_sink& input_sink)
{
	if (!valid_)
	{
		return false;
	}

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
 //   SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

	window_ = SDL_CreateWindow(title.c_str(), 0, 0, size.x, size.y, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!window_)
    {
        return false;
    }

	SDL_GLContext gl_context = SDL_GL_CreateContext(window_);

	SDL_GL_MakeCurrent(window_, gl_context);

    show_system_cursor(false);

	size_ = size;

	input_sink_ = &input_sink;

	return true;
}

SDL_Window* Client::get_handle() const
{
	return window_;
}

void Client::update()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (SDL_WINDOWEVENT == event.type)
        {
            switch (event.window.event)
            {
                case SDL_WINDOWEVENT_CLOSE:
					messages_.push(Message::Quit);
                    break;
                default:
                    break;
               }

        }
        else if (SDL_KEYDOWN == event.type || SDL_KEYUP == event.type)
        {
            on_input_signal(Input_signal(event.key.keysym.scancode, SDL_KEYDOWN == event.type ? 1 : 0));
        }
        else if (SDL_TEXTINPUT == event.type)
        {
            on_input_signal(Input_signal(event.text.text[0]));
        }
        else if (SDL_MOUSEMOTION == event.type)
        {
            on_input_signal(Input_signal(platform::motion::Mouse_x, event.motion.xrel, false));
            on_input_signal(Input_signal(platform::motion::Mouse_y, event.motion.yrel, false));
        }
        else if (SDL_MOUSEBUTTONDOWN == event.type || SDL_MOUSEBUTTONUP == event.type)
        {
            int state = SDL_MOUSEBUTTONDOWN == event.type ? 1 : 0;
            uint32_t button;

            switch (event.button.button)
            {
            case SDL_BUTTON_LEFT:
                button = platform::key::Mouse_left;
                break;
            case SDL_BUTTON_RIGHT:
                button = platform::key::Mouse_right;
                break;
            }

            on_input_signal(Input_signal(button, state));
        }

    }
}

Message Client::get_message()
{
	if (!messages_.empty())
    {
		const Message message = messages_.top();

		messages_.pop();

        return message;
    }
    else
    {
        return Message::Nothing;
    }
}

const uint2& Client::get_size() const
{
	return size_;
}

void Client::show_system_cursor(bool show)
{
    SDL_SetRelativeMouseMode(show ? SDL_FALSE : SDL_TRUE);
}

float2 Client::get_relative_cursor_position() const
{
    int x;
    int y;

    SDL_GetMouseState(&x, &y);

	return float2(float(x), float(y));
}

void Client::set_title(const std::string& title)
{
	SDL_SetWindowTitle(window_, title.c_str());
}

void Client::on_input_signal(const Input_signal& signal)
{
	input_sink_->on_input_signal(signal);
}

void Client::add_message(Message message)
{
	messages_.push(message);
}
/*
void close_callback(GLFWwindow* window)
{
	reinterpret_cast<Client*>(glfwGetWindowUserPointer(window))->add_message(Message::Quit);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	reinterpret_cast<Client*>(glfwGetWindowUserPointer(window))->on_input_signal(Input_signal(key, action));
}

void character_callback(GLFWwindow* window, unsigned int character)
{
	reinterpret_cast<Client*>(glfwGetWindowUserPointer(window))->on_input_signal(Input_signal(character));
}
*/
}
