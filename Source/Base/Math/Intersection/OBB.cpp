#include "OBB.hpp"
#include "Math/Vector.inl"
#include "Math/Matrix.inl"

OBB::OBB()
{}

OBB::OBB(const float3& position, const float3& scale, const float3x3& rotation) :
	position(position),
	halfsize_x(float3(scale.x, 0.f, 0.f) * rotation),
	halfsize_y(float3(0.f, scale.y, 0.f) * rotation),
	halfsize_z(float3(0.f, 0.f, scale.z) * rotation)
{}

OBB::OBB(const float4x4& m) : position(m.rows[3].xyz), halfsize_x(m.rows[0].xyz), halfsize_y(m.rows[1].xyz), halfsize_z(m.rows[2].xyz)
{}
