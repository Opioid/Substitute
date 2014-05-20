#include "OBB.hpp"


OBB::OBB()
{}


OBB::OBB(const float3& position, const float3& scale, const float3x3& rotation) 
	: position(position),
	  m_halfsize_x(float3(scale.x, 0.f, 0.f) * rotation),
	  m_halfsize_y(float3(0.f, scale.y, 0.f) * rotation),
	  m_halfsize_z(float3(0.f, 0.f, scale.z) * rotation)
{}


OBB::OBB(const float4x4& m) : position(m.rows[3].xyz), m_halfsize_x(m.rows[0].xyz), m_halfsize_y(m.rows[1].xyz), m_halfsize_z(m.rows[2].xyz)
{}