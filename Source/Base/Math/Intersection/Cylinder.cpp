#include "Cylinder.hpp"


Cylinder::Cylinder()
{}


Cylinder::Cylinder(const float3& pos, const float3& axis, float radius, float halfHeight) : position(pos), m_axis(axis), m_radius(radius), m_halfHeight(halfHeight)
{}