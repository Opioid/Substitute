#pragma once

#include "Math/Transformation.hpp"
#include "Math/Vector.hpp"
#include "Math/Quaternion.hpp"

namespace scene
{

class Entity;

class Transformation_interpolator
{

public:

	Transformation_interpolator(Entity* entity);

	void copy_previous_state();
	void interpolate(float delta);

private:

	Entity* entity_;

	Transformation transformation_;
};

}
