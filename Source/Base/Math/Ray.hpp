#pragma once

#include "Vector.hpp"

struct Plane;
struct Sphere;
class Cylinder;

struct Ray3
{
	float3 org;
	float3 dir;

	Ray3();
	Ray3(const float3& org, const float3& dir);

	bool intersect(const Plane& p, float &dist) const;
	bool intersect(const Sphere& sphere) const;
	bool intersect(const Cylinder &cylinder) const;

	bool intersectTriangle(const float3& v0, const float3& v1, const float3& v2, float &t) const;
	bool intersectQuad(const float3& v0, const float3& v1, const float3& v2, const float3& v3, float &t) const;
};
