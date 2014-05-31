#include "AABB.hpp"
#include "Math/Vector.inl"
#include "Math/Matrix.inl"
#include "Math/Plane.hpp"
#include "Math/Math.hpp"

AABB::AABB()
{}

AABB::AABB(const float3& min, const float3& max) : position(0.5f * (min + max)), halfsize(0.5f * (max - min))
{}

float3 AABB::get_min() const
{
	return position - halfsize;
}

float3 AABB::get_max() const
{
	return position + halfsize;
}

void AABB::set_min_max(const float3& min, const float3& max)
{
	halfsize = 0.5f * (max - min);
	position = 0.5f * (min + max);
}

void AABB::get_transformed(AABB& aabb, const float4x4& m) const
{
	aabb.halfsize.x = std::abs(m.m00) * halfsize.x + std::abs(m.m10) * halfsize.y + std::abs(m.m20) * halfsize.z;
	aabb.halfsize.y = std::abs(m.m01) * halfsize.x + std::abs(m.m11) * halfsize.y + std::abs(m.m21) * halfsize.z;
	aabb.halfsize.z = std::abs(m.m02) * halfsize.x + std::abs(m.m12) * halfsize.y + std::abs(m.m22) * halfsize.z;

	aabb.position = position * m;
}

AABB AABB::merge(const AABB& aabb) const
{
	float3 amin = get_min();
	float3 amax = get_max();

	float3 bmin = aabb.get_min();
	float3 bmax = aabb.get_max();

	if (amin.x > bmin.x) amin.x = bmin.x;
	if (amin.y > bmin.y) amin.y = bmin.y;
	if (amin.z > bmin.z) amin.z = bmin.z;

	if (amax.x < bmax.x) amax.x = bmax.x;
	if (amax.y < bmax.y) amax.y = bmax.y;
	if (amax.z < bmax.z) amax.z = bmax.z;
		
	return AABB(amin, amax);
}

void AABB::get_corners(float3 corners[8]) const
{
	float3 min = get_min();
	float3 max = get_max();

	corners[0] = float3(min.x, min.y, min.z);
	corners[1] = float3(min.x, min.y, max.z);
	corners[2] = float3(min.x, max.y, min.z);
	corners[3] = float3(min.x, max.y, max.z);
	corners[4] = float3(max.x, min.y, min.z);
	corners[5] = float3(max.x, min.y, max.z);
	corners[6] = float3(max.x, max.y, min.z);
	corners[7] = float3(max.x, max.y, max.z);
}

Intersection_type AABB::intersect(const AABB& aabb) const
{
	float3 amin = get_min();
	float3 amax = get_max();

	float3 bmin = aabb.get_min();
	float3 bmax = aabb.get_max();

	if (amin.x <= bmin.x && amax.x >= bmax.x 
	&&  amin.y <= bmin.y && amax.y >= bmax.y 
	&&  amin.z <= bmin.z && amax.z >= bmax.z)
	{
		return Intersection_type::Inside;
	}

	return Intersection_type::Outside;
}

Intersection_type AABB::intersect(const float3& point) const
{
	float3 min = get_min();
	float3 max = get_max();

	if (point.x >= min.x && point.y >= min.y && point.z >= min.z
	&&  point.x <= max.x && point.y <= max.y && point.z <= max.z)
	{
		return Intersection_type::Inside;
	}

	return Intersection_type::Outside;
}

Intersection_type AABB::intersect(const Plane& plane) const
{
	float m = dot(plane, position);
	float n = halfsize.x * std::abs(plane.a) + halfsize.y * std::abs(plane.b) + halfsize.z * std::abs(plane.c);

	if (m + n < 0.f) return Intersection_type::Outside;
	if (m - n < 0.f) return Intersection_type::Intersecting;

	return Intersection_type::Inside;
}
