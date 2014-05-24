#pragma once

#include "Input_signal_queue.hpp"
#include "Input_state.hpp"
#include "Control_mappings.hpp"

class Controls
{

public:

	typedef std::vector<platform::Input_signal> Signal_stream;

	Controls();

	Control_mappings& mappings();

	void on_input_signal(const platform::Input_signal& signal);

	void update(float speed);
	void cleanup_update();

	const Input_state& mouse_state() const;
	const Input_state& keyboard_state() const;

	const Signal_stream& signal_stream() const;

private:
	
	Input_signal_queue signal_queue_;

	Input_state keyboard_;
	Input_state mouse_;

	Signal_stream signal_stream_;

	Control_mappings mappings_;
};
