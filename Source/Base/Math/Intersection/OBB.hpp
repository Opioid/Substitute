#pragma once

#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"

struct OBB
{
	OBB();
	OBB(const float3& position, const float3& scale, const float3x3& rotation);
	OBB(const float4x4& m);

	float3 position;
	float3 halfsize_x;
	float3 halfsize_y;
	float3 halfsize_z;
};
