#include "Sphere.hpp"
#include "AABB.hpp"
#include "OBB.hpp"
#include "../Plane.hpp"

Sphere::Sphere()
{}

Sphere::Sphere(const float3& pos, float radius) : position(pos), radius(radius)
{}

void Sphere::scale(float scale)
{
	radius *= scale;
}

void Sphere::translate(const float3& v)
{
	position += v;
};

Intersection_type Sphere::intersect(const float3& point) const
{
	if (squared_distance(position, point) <= radius * radius)
	{
		return Intersection_type::Inside;
	}
		
	return Intersection_type::Outside;
}

Intersection_type Sphere::intersect(const AABB& aabb) const
{
	float3 min = aabb.get_min();
	float3 max = aabb.get_max();

	float d = 0.f; 

	for (size_t i = 0; i < 3; ++i)
	{
		if (position.v[i] < min.v[i])
		{
			float s = position.v[i] - min.v[i];
			d += s * s; 
		}
		else if (position.v[i] > max.v[i])
		{
			float s = position.v[i] - max.v[i];
			d += s * s; 
		}
	}

	if (d <= radius * radius)
	{
		return Intersection_type::Intersecting;
	}
	else
	{
		return Intersection_type::Outside;
	}
}

Intersection_type Sphere::intersect(const OBB& obb) const
{
	float3 nhx = normalize(obb.halfsize_x);
	float3 nhy = normalize(obb.halfsize_y);
	float3 nhz = normalize(obb.halfsize_z);

	{
		Plane plane(nhx, obb.position + obb.halfsize_x);
		if (dot(plane, position) > radius)
		{
			return Intersection_type::Outside;
		}
	}

	{
		Plane plane(nhy, obb.position + obb.halfsize_y);
		if (dot(plane, position) > radius)
		{
			return Intersection_type::Outside;
		}
	}

	{
		Plane plane(nhz, obb.position + obb.halfsize_z);
		if (dot(plane, position) > radius)
		{
			return Intersection_type::Outside;
		}
	}

	{
		Plane plane(-nhx, obb.position - obb.halfsize_x);
		if (dot(plane, position) > radius)
		{
			return Intersection_type::Outside;
		}
	}

	{
		Plane plane(-nhy, obb.position - obb.halfsize_y);
		if (dot(plane, position) > radius)
		{
			return Intersection_type::Outside;
		}
	}

	{
		Plane plane(-nhz, obb.position - obb.halfsize_z);
		if (dot(plane, position) > radius)
		{
			return Intersection_type::Outside;
		}
	}

	return Intersection_type::Intersecting;
}

