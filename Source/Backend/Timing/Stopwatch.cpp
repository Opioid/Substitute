#include "Stopwatch.hpp"
#include "../Platform/Platform.hpp"

namespace timing
{

Stopwatch::Stopwatch() : m_start_time(0), m_elapsed_time(0.0)
{
    m_frequency = static_cast<double>(SDL_GetPerformanceFrequency());
}

void Stopwatch::start()
{
    m_start_time = get_current_time();
}

double Stopwatch::update()
{
    uint64_t current_time = get_current_time();

    uint64_t interval = current_time - m_start_time;

    m_start_time = current_time;

    m_elapsed_time = static_cast<double>(interval) / m_frequency;

    return m_elapsed_time;
}

double Stopwatch::stop()
{
    return update();
}

double Stopwatch::get_elapsed_time() const
{
        return m_elapsed_time;
}

uint64_t Stopwatch::get_current_time() const
{
        return SDL_GetPerformanceCounter();
}

}
