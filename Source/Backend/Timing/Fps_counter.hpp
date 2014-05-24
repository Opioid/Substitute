#pragma once

#include "Stopwatch.hpp"
#include <cstddef>

namespace timing
{

class Fps_counter
{

public:

	Fps_counter();

	void reset();

	void update();

	double frame_time() const;

	size_t fps() const;

private:

    Stopwatch m_watch;

	bool m_first_call;

	double m_frame_time;
	double m_frame_time_counter;

	size_t m_frame_counter;
	size_t m_fps;
};

}
