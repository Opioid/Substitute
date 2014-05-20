#pragma once

#include "Math/Vector.hpp"
#include "Rendering/Color.hpp"

namespace rendering
{

namespace baking
{

struct Ambient_cube
{
	void clear(const Color3& color)
	{
		for (size_t i = 0; i < 6; ++i)
		{
			colors[i] = color;
		}
	}

	Ambient_cube& operator+=(const Ambient_cube& other)
	{
		for (size_t i = 0; i < 6; ++i)
		{
			colors[i] += other.colors[i];
		}

		return *this;
	}

	Color3 colors[6];
};

class Ambient_cube_volume
{

public:

	Ambient_cube_volume(uint32_t num_cubes);
	~Ambient_cube_volume();

	const Ambient_cube& get_ambient_cube(uint32_t index) const;
	Ambient_cube& get_ambient_cube(uint32_t index);

private:

	uint32_t num_cubes_;

	Ambient_cube* cubes_;
};

}

}
