#pragma once

#include "Intersection_type.hpp"
#include "Math/Vector.hpp"

struct AABB;
class OBB;

struct Sphere
{

public:

	Sphere();
	Sphere(const float3& pos, float radius);
	
	void scale(float scale);
	void translate(const float3& v);

	Intersection_type::Value intersect(const float3& point) const;
	Intersection_type::Value intersect(const AABB& aabb) const;
	Intersection_type::Value intersect(const OBB& obb) const;
	
	float3 position;
	float  radius;
};
