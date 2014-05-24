#include "Stopwatch.hpp"
#include "Platform/Platform.hpp"

namespace timing
{

Stopwatch::Stopwatch() : start_time_(0), elapsed_time_(0.0)
{
	frequency_ = static_cast<double>(SDL_GetPerformanceFrequency());
}

void Stopwatch::start()
{
	start_time_ = current_time();
}

double Stopwatch::update()
{
	const uint64_t now = current_time();

	const uint64_t interval = now - start_time_;

	elapsed_time_ = static_cast<double>(interval) / frequency_;

	start_time_ = now;

	return elapsed_time_;
}

double Stopwatch::stop()
{
    return update();
}

double Stopwatch::elapsed_time() const
{
	return elapsed_time_;
}

uint64_t Stopwatch::current_time() const
{
	return SDL_GetPerformanceCounter();
}

}
