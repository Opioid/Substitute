#pragma once

#include "Platform/Input_signal.hpp"
#include <queue>

class Input_signal_queue
{
	
public:

	void                   push(const platform::Input_signal& signal);
	platform::Input_signal pop();

	bool is_signal_left() const;

private:

	std::queue<platform::Input_signal> m_signals;
};
