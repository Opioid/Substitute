#pragma once

#include "Node.hpp"
#include "Flags/Flags.hpp"

namespace scene
{

class Entity : public Node
{

public:

	enum class State
	{
		Visible		 = 1 << 0,
		Interpolated = 1 << 1,
	};

	typedef Flags<State, uint8_t> Entity_state;
	
	Entity();

	bool is_visible() const;
	void set_visible(bool visible);

	bool is_interpolated() const;
	void set_interpolated(bool interpolated);

private:

	Entity_state state_;
};

}
