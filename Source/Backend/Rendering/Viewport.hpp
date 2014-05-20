#pragma once

#include "Math/Vector.hpp"

namespace rendering
{

struct Viewport
{
	Viewport();
	Viewport(const float2& origin, const float2& size);
	Viewport(const float2& origin, const float2& size, const float2& depth_range);

	float2 origin;
	float2 size;
	float2 depth_range;
};

}
