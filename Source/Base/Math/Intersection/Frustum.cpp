#include "Frustum.hpp"
#include "Sphere.hpp"
#include "AABB.hpp"
#include "OBB.hpp"
#include <cmath>

Frustum::Frustum()
{}

Frustum::Frustum(const float4x4& combo_matrix)
{
	set_from_matrix(combo_matrix);
}

void Frustum::set_from_matrix(const float4x4& combo_matrix)
{
	// Left clipping plane
	planes_[0].a = combo_matrix.m03 + combo_matrix.m00;
	planes_[0].b = combo_matrix.m13 + combo_matrix.m10;
	planes_[0].c = combo_matrix.m23 + combo_matrix.m20;
	planes_[0].d = combo_matrix.m33 + combo_matrix.m30;
	planes_[0] = normalize(planes_[0]);

	// Right clipping plane
	planes_[1].a = combo_matrix.m03 - combo_matrix.m00;
	planes_[1].b = combo_matrix.m13 - combo_matrix.m10;
	planes_[1].c = combo_matrix.m23 - combo_matrix.m20;
	planes_[1].d = combo_matrix.m33 - combo_matrix.m30;
	planes_[1] = normalize(planes_[1]);

	// Top clipping plane
	planes_[2].a = combo_matrix.m03 - combo_matrix.m01;
	planes_[2].b = combo_matrix.m13 - combo_matrix.m11;
	planes_[2].c = combo_matrix.m23 - combo_matrix.m21;
	planes_[2].d = combo_matrix.m33 - combo_matrix.m31;
	planes_[2] = normalize(planes_[2]);

	// Bottom clipping plane
	planes_[3].a = combo_matrix.m03 + combo_matrix.m01;
	planes_[3].b = combo_matrix.m13 + combo_matrix.m11;
	planes_[3].c = combo_matrix.m23 + combo_matrix.m21;
	planes_[3].d = combo_matrix.m33 + combo_matrix.m31;
	planes_[3] = normalize(planes_[3]);

	// Near clipping plane
#ifdef CLIP_NEAR_Z_MINUS_ONE
	planes_[4].a = combo_matrix.m03 + combo_matrix.m02;
	planes_[4].b = combo_matrix.m13 + combo_matrix.m12;
	planes_[4].c = combo_matrix.m23 + combo_matrix.m22;
	planes_[4].d = combo_matrix.m33 + combo_matrix.m32;
#else
	planes_[4].a = combo_matrix.m02;
	planes_[4].b = combo_matrix.m12;
	planes_[4].c = combo_matrix.m22;
	planes_[4].d = combo_matrix.m32;
#endif
	planes_[4] = normalize(planes_[4]);

	// Far clipping plane
	planes_[5].a = combo_matrix.m03 - combo_matrix.m02;
	planes_[5].b = combo_matrix.m13 - combo_matrix.m12;
	planes_[5].c = combo_matrix.m23 - combo_matrix.m22;
	planes_[5].d = combo_matrix.m33 - combo_matrix.m32;
	planes_[5] = normalize(planes_[5]);

	calculate_points();
}

void Frustum::set_plane(size_t index, const Plane& plane)
{
	planes_[index] = plane;
}

void Frustum::calculate_points()
{
	points_[0] = Plane::intersection(planes_[0], planes_[3], planes_[4]);	//Left  Bottom  Near
	points_[1] = Plane::intersection(planes_[1], planes_[3], planes_[4]);	//Right Bottom  Near

	points_[2] = Plane::intersection(planes_[0], planes_[2], planes_[4]);	//Left  Upper Near
	points_[3] = Plane::intersection(planes_[1], planes_[2], planes_[4]);	//Right Upper Near

	points_[4] = Plane::intersection(planes_[0], planes_[3], planes_[5]);	//Left  Bottom  Far
	points_[5] = Plane::intersection(planes_[1], planes_[3], planes_[5]);	//Right Bottom  Far

	points_[6] = Plane::intersection(planes_[0], planes_[2], planes_[5]);	//Left  Upper Far
	points_[7] = Plane::intersection(planes_[1], planes_[2], planes_[5]);	//Right Upper Far
}

Intersection_type::Value Frustum::intersect(const float3& point) const
{
	for (size_t i = 0; i < 6; ++i)
	{
		if (behind(planes_[i], point))
		{
			return Intersection_type::Outside;
		}
	}

	return Intersection_type::Inside;
}

Intersection_type::Value Frustum::intersect(const Sphere& sphere) const
{
	for (size_t i = 0; i < 6; ++i)
	{
		if (dot(planes_[i], sphere.position) < -sphere.radius)
		{
			return Intersection_type::Outside;
		}
	}

	return Intersection_type::Intersecting;
}

Intersection_type::Value Frustum::intersect(const AABB& aabb) const
{
	Intersection_type::Value result = Intersection_type::Inside;

	for (size_t i = 0; i < 6; ++i)
	{
		float m = dot(planes_[i], aabb.position);
		float n = aabb.halfsize.x * std::abs(planes_[i].a) + aabb.halfsize.y * std::abs(planes_[i].b) + aabb.halfsize.z * std::abs(planes_[i].c);

		if (m + n < 0.f)
		{
			return Intersection_type::Outside;
		}

		if (m - n < 0.f) 
		{
			result = Intersection_type::Intersecting;
		}
	} 

	return result;
}

Intersection_type::Value Frustum::intersect(const OBB& obb) const
{
	for (size_t i = 0; i < 6; ++i)
	{
		float m = dot(planes_[i], obb.position);

		if (m < 0.f)
		{
			float n = std::abs(dot(planes_[i].normal(), obb.m_halfsize_x)) + std::abs(dot(planes_[i].normal(), obb.m_halfsize_y)) + std::abs(dot(planes_[i].normal(), obb.m_halfsize_z));

			if (m < -n)
			{
				return Intersection_type::Outside;
			}
		}
	}

	return Intersection_type::Intersecting;
}

// Line-frustum intersection
Intersection_type::Value Frustum::intersect(const float3& p0, const float3& p1) const
{
	Intersection_type::Value result = Intersection_type::Inside;

	for (size_t i = 0; i < 6; ++i)
	{	
		float m = dot(planes_[i], p0);
		float n = dot(planes_[i], p1); 

		if (m + n < 0.f)
		{
			return Intersection_type::Outside;
		}

		if (m - n < 0.f) 
		{
			result = Intersection_type::Intersecting;
		}
	}

	return result;
}

Intersection_type::Value Frustum::intersect(const Frustum& frustum) const
{
	for (size_t i = 0; i < 6; ++i)
	{
		if (behind(planes_[i], frustum.points_[0])
		&&  behind(planes_[i], frustum.points_[1])
		&&  behind(planes_[i], frustum.points_[2])
		&&  behind(planes_[i], frustum.points_[3])
		&&  behind(planes_[i], frustum.points_[4])
		&&  behind(planes_[i], frustum.points_[5])
		&&  behind(planes_[i], frustum.points_[6])
		&&  behind(planes_[i], frustum.points_[7]))
		{
			return Intersection_type::Outside;
		}
	}

	return Intersection_type::Intersecting;
}

AABB Frustum::calculate_AABB() const
{               
	float3 min( 100000.f,  100000.f,  100000.f);
	float3 max(-100000.f, -100000.f, -100000.f);

	for (size_t i = 0; i < 8; ++i)
	{
		min = math::min(min, points_[i]);
		max = math::max(max, points_[i]);
	/*	if (min.x > points[i].x) min.x = points[i].x;
		if (min.y > points[i].y) min.y = points[i].y;
		if (min.z > points[i].z) min.z = points[i].z;

		if (max.x < points[i].x) max.x = points[i].x;
		if (max.y < points[i].y) max.y = points[i].y;
		if (max.z < points[i].z) max.z = points[i].z;
		*/
	}

	return AABB(min, max);
}
