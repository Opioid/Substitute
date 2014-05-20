#include "Input_state.hpp"
#include "Platform/Input_signal.hpp"
#include <cstring>

Input_state::Input_state(size_t num_states, size_t num_axes) : m_num_axes(num_axes)
{
	m_key_down.resize(num_states);
	m_key_just_pressed.resize(num_states);

	for (size_t i = 0; i < num_states; ++i)
	{
		m_key_down[i] = false;
		m_key_just_pressed[i] = false;
	}

	if (m_num_axes)
	{
		m_axes = new int[m_num_axes];
	}
	else
	{
		m_axes = nullptr;
	}
}

Input_state::~Input_state()
{
	delete [] m_axes;
}

void Input_state::cleanup_update()
{
	for (size_t i = 0; i < m_key_just_pressed.size(); ++i)
	{
		m_key_just_pressed[i] = false;
	}

	memset(m_axes, 0, sizeof(int) * m_num_axes);
}

void Input_state::set_key_down(uint32_t key)
{
	m_key_just_pressed[key] = !m_key_down[key];

	m_key_down[key] = true;
}

void Input_state::set_key_up(uint32_t key)
{
	m_key_down[key] = false;
}

void Input_state::set_axis(size_t axis, int d)
{
	m_axes[axis] = d;
}

bool Input_state::is_key_down(uint32_t key) const
{
	return m_key_down[key];
}

bool Input_state::is_key_click(uint32_t key) const
{
	return m_key_down[key] && m_key_just_pressed[key];
}

int Input_state::get_axis(size_t axis) const
{
	return m_axes[axis];
}
