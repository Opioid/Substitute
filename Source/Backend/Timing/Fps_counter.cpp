#include "Fps_counter.hpp"

namespace timing
{

Fps_counter::Fps_counter() : m_first_call(true), m_frame_time(0.0), m_frame_time_counter(0.0), m_frame_counter(0), m_fps(0)
{}

void Fps_counter::reset()
{
	m_first_call = true;
}

void Fps_counter::update()
{
	m_frame_time = m_watch.update();

	if (m_first_call)
	{
		m_frame_time = 0.0;
		m_first_call = false;
	}

	m_frame_time_counter += m_frame_time;

	if (m_frame_time_counter >= 1.0)
	{
		m_frame_time_counter = 0.0;
		m_fps = m_frame_counter;
		m_frame_counter = 0;
	}

	++m_frame_counter;
}

double Fps_counter::get_frame_time() const
{
	return m_frame_time;
}

size_t Fps_counter::get_fps() const
{
	return m_fps;
}

}
