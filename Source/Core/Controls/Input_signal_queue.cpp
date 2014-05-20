#include "Input_signal_queue.hpp"

void Input_signal_queue::push(const platform::Input_signal& signal)
{
	m_signals.push(signal);
}

platform::Input_signal Input_signal_queue::pop()
{
	platform::Input_signal signal = m_signals.front();
	m_signals.pop();
	return signal;
}

bool Input_signal_queue::is_signal_left() const
{
	return !m_signals.empty();
}
