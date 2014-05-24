#pragma once

#include "Node.hpp"

namespace scene
{

class Entity : public Node
{

public:

	struct Flags
	{
		enum Value
		{
			Visible = 1,
			Interpolated = 2
		};
	};
	
	Entity();

	bool is_visible() const;
	void set_visible(bool visible);

	bool is_interpolated() const;
	void set_interpolated(bool interpolated);

private:

	unsigned char flags_;
};

}
