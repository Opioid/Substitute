#pragma once

#include <vector>
#include <cstdint>
#include <cstddef>

class Input_state
{

public:

	Input_state(size_t num_states, size_t num_axes = 0);
	~Input_state();

	void cleanup_update();

	void set_key_down(uint32_t key);
	void set_key_up(uint32_t key);
	void set_axis(size_t axis, int d);

	bool is_key_down(uint32_t key) const;
	bool is_key_click(uint32_t key) const;

	int get_axis(size_t axis) const;

private:

	std::vector<bool> m_key_down;
	std::vector<bool> m_key_just_pressed;

	const size_t m_num_axes;
	int* m_axes;
};
