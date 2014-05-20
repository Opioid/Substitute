#pragma once

#include "Math/Vector.hpp"
#include "Math/Ray.hpp"
#include <cstdint>

namespace scene
{

class Camera;

}

class Cursor
{

public:

	void update(const float2& coords);

	const float2& get_coord() const;
	const float2& get_previous_coord() const;

	Ray3 get_ray(const scene::Camera& camera, const uint2& screen_size) const;

private:

	float2 coords_;
	float2 previous_coords_;
};
