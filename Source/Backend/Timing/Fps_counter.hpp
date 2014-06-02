#pragma once

#include "Stopwatch.hpp"

namespace timing
{

class Fps_counter
{

public:

	Fps_counter();

	void reset();

	void update();

	double frame_time() const;

	uint32_t fps() const;

private:

	Stopwatch watch_;

	bool first_call_;

	double frame_time_;
	double frame_time_counter_;

	uint32_t frame_counter_;
	uint32_t fps_;
};

}
