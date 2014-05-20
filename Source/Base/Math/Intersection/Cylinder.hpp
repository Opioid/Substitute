#pragma once


#include "../Vector.hpp"


class Cylinder
{
public:

	Cylinder();
	Cylinder(const float3& pos, const float3& axis, float radius, float halfHeight);

	float3 position;
	float3 m_axis;
	float   m_radius;
	float   m_halfHeight;
};