#pragma once


#include "../Vector.hpp"
#include "../Matrix.hpp"


class OBB
{
public:

	OBB();
	OBB(const float3& position, const float3& scale, const float3x3& rotation);
	OBB(const float4x4& m);


	float3 position;
	float3 m_halfsize_x;
	float3 m_halfsize_y;
	float3 m_halfsize_z;
};