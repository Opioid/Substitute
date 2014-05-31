#pragma once

#include "Intersection_type.hpp"
#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"

struct Plane;

struct AABB
{
	AABB();
	AABB(const float3& min, const float3& max);

	float3 get_min() const;
	float3 get_max() const;

	void set_min_max(const float3& min, const float3& max);

	void get_transformed(AABB& aabb, const float4x4& m) const;
	AABB merge(const AABB& aabb) const;

	void get_corners(float3 corners[8]) const;

	Intersection_type intersect(const AABB& aabb) const;
	Intersection_type intersect(const float3& point) const;
	Intersection_type intersect(const Plane& plane) const;

	float3 position;
	float3 halfsize;
};
