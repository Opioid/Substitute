#include "Fps_counter.hpp"

namespace timing
{

Fps_counter::Fps_counter() : first_call_(true), frame_time_(0.0), frame_time_counter_(0.0), frame_counter_(0), fps_(0)
{}

void Fps_counter::reset()
{
	first_call_ = true;
}

void Fps_counter::update()
{
	frame_time_ = watch_.update();

	if (first_call_)
	{
		frame_time_ = 0.0;
		first_call_ = false;
	}

	frame_time_counter_ += frame_time_;

	if (frame_time_counter_ >= 1.0)
	{
		frame_time_counter_ = 0.0;
		fps_ = frame_counter_;
		frame_counter_ = 0;
	}

	++frame_counter_;
}

double Fps_counter::frame_time() const
{
	return frame_time_;
}

uint32_t Fps_counter::fps() const
{
	return fps_;
}

}
