#pragma once

#include "Intersection_type.hpp"
#include "../Plane.hpp"

struct Ray3;

struct Sphere;
struct AABB;
class OBB;

class Frustum
{
		
public:

	struct Planes
	{
		enum Value
		{
			Left,
			Right,
			Top,
			Bottom,
			Near,
			Far
		};
	};


	Frustum();
	Frustum(const float4x4& combo_matrix);

	void set_from_matrix(const float4x4& combo_matrix);
	void set_plane(size_t index, const Plane& plane);

	void calculate_points();

	Intersection_type::Value intersect(const float3& point) const;

	Intersection_type::Value intersect(const Sphere& sphere) const;

	Intersection_type::Value intersect(const AABB& aabb) const;

	Intersection_type::Value intersect(const OBB& obb) const;

	Intersection_type::Value intersect(const float3& p0, const float3& p1) const;

	Intersection_type::Value intersect(const Frustum& frustum) const;

	AABB calculate_AABB() const;

	Plane planes_[6];
	float3 points_[8];
};
