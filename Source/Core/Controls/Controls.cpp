#include "Controls.hpp"

Controls::Controls() : keyboard_(256), mouse_(3, 3)
{}

Control_mappings& Controls::mappings()
{
	return mappings_;
}

void Controls::on_input_signal(const platform::Input_signal& signal)
{
	signal_queue_.push(signal);
}

void Controls::update(float speed)
{
	while (signal_queue_.is_signal_left())
	{
		platform::Input_signal signal = signal_queue_.pop();

		signal_stream_.push_back(signal);

		unsigned int id = signal.id();

		if (platform::key::Mouse_left   == id
		||  platform::key::Mouse_right  == id
        ||  platform::key::Mouse_middle == id)
        {
            if (signal.is_pressed())
            {
				mouse_.set_key_down(id - platform::key::Mouse_left);
            }
            else
            {
				mouse_.set_key_up(id - platform::key::Mouse_left);
            }
        }
        else if (
            platform::motion::Mouse_x == id
        ||  platform::motion::Mouse_y == id)
        {
			mouse_.set_axis(id - platform::motion::Mouse_x, signal.data());
        }
        else if (id < 256)
        {
            if (signal.is_pressed())
            {
				keyboard_.set_key_down(id);
            }
            else
            {
				keyboard_.set_key_up(id);
            }
        }

		mappings_.insert(signal);
	}

	mappings_.process(speed);
}

void Controls::cleanup_update()
{
	keyboard_.cleanup_update();
	mouse_.cleanup_update();

	signal_stream_.clear();
}

const Input_state& Controls::mouse_state() const
{
	return mouse_;
}

const Input_state& Controls::keyboard_state() const
{
	return keyboard_;
}


const Controls::Signal_stream& Controls::signal_stream() const
{
	return signal_stream_;
}
