#include "Polyhedron.hpp"
#include "AABB.hpp"
#include <cmath>

#include "String/String.hpp"
#include <iostream>

void Polyhedron::add_plane(const Plane& plane)
{
	planes_.push_back(plane);
}

Intersection_type Polyhedron::intersect(const AABB& aabb) const
{
	Intersection_type result = Intersection_type::Inside;

	for (size_t i = 0; i < planes_.size(); ++i) 
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

Intersection_type Polyhedron::intersect(const AABB& aabb, size_t& out) const
{
	Intersection_type result = Intersection_type::Inside;

	for (size_t i = 0; i < planes_.size(); ++i) 
	{
		out = i;
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

// the next 2 functions were implemented according to this:
// http://www.gamedev.net/reference/articles/article2330.asp

Polyhedron Polyhedron::create_point_shadow_caster_volume(const Frustum& frustum, const float3& position)
{
	Polyhedron polyhedron;

	// the point shadow caster is inside the frustum, thus the shadow caster volume IS the frustum
	if (Intersection_type::Outside != frustum.intersect(position))
	{
		for (size_t i = 0; i < 6; ++i)
		{
			polyhedron.add_plane(frustum.planes_[i]);
		}
	}
	else
	{
		// planes facing the light remain
		for (size_t i = 0; i < 6; ++i)
		{
//			if (i != 3)
//			{
//				continue;
//			}

			if (!behind(frustum.planes_[i], position))
			{
				polyhedron.add_plane(frustum.planes_[i]);
			}

		//	std::cout << frustum.planes_[i].normal() << std::endl;
		}

		Edge edges[12];

		//edges with the left frustum plane
		edges[0].p0 = frustum.planes_[0];
		edges[0].p1 = frustum.planes_[2];
		edges[0].v0 = frustum.points_[2]; //left, top, near
		edges[0].v1 = frustum.points_[6]; //left, top, far
			
		edges[1].p0 = frustum.planes_[0];
		edges[1].p1 = frustum.planes_[3];
		edges[1].v0 = frustum.points_[0];	//left, bottom, near
		edges[1].v1 = frustum.points_[4]; //left, bottom, far

		edges[2].p0 = frustum.planes_[0];
		edges[2].p1 = frustum.planes_[4];
		edges[2].v0 = edges[0].v0; //left, near, top
		edges[2].v1 = edges[1].v0; //left, near, bottom

		edges[3].p0 = frustum.planes_[0];
		edges[3].p1 = frustum.planes_[5];
		edges[3].v0 = edges[0].v1; //left, far, top
		edges[3].v1 = edges[1].v1; //left, far, bottom

		//edges with the right frustum plane
		edges[4].p0 = frustum.planes_[1];
		edges[4].p1 = frustum.planes_[2];
		edges[4].v0 = frustum.points_[3];	//right, top, near
		edges[4].v1 = frustum.points_[7]; //right, top, far

		edges[5].p0 = frustum.planes_[1];
		edges[5].p1 = frustum.planes_[3];
		edges[5].v0 = frustum.points_[1]; //right, bottom, near
		edges[5].v1 = frustum.points_[5]; //right, bottom, far

		edges[6].p0 = frustum.planes_[1];
		edges[6].p1 = frustum.planes_[4];
		edges[6].v0 = edges[4].v0; //right, near, top
		edges[6].v1 = edges[5].v0; //right, near, bottom

		edges[7].p0 = frustum.planes_[1];
		edges[7].p1 = frustum.planes_[5];
		edges[7].v0 = edges[4].v1; //right, far, top
		edges[7].v1 = edges[5].v1; //right, far, bottom

		//edges with the top frustum plane
		edges[8].p0 = frustum.planes_[2];
		edges[8].p1 = frustum.planes_[4];
		edges[8].v0 = edges[0].v0; //top, near, left
		edges[8].v1 = edges[4].v0; //top, near, right

		edges[9].p0 = frustum.planes_[2];
		edges[9].p1 = frustum.planes_[5];
		edges[9].v0 = edges[0].v1; //top, far, left
		edges[9].v1 = edges[4].v1; //top, far, right

		//edges with the bottom frustum plane
		edges[10].p0 = frustum.planes_[3];
		edges[10].p1 = frustum.planes_[4];
		edges[10].v0 = edges[1].v0; //bottom, near, left
		edges[10].v1 = edges[5].v0; //bottom, near, right

		edges[11].p0 = frustum.planes_[3];
		edges[11].p1 = frustum.planes_[5];
		edges[11].v0 = edges[1].v1; //bottom, far, left
		edges[11].v1 = edges[5].v1; //bottom, far, right

		float3 inside_frustum = (edges[0].v0 + edges[0].v1 + edges[1].v0 + edges[1].v1
							  +  edges[4].v0 + edges[4].v1 + edges[5].v0 + edges[5].v1) / 8.f;

		for (size_t i = 0; i < 12; ++i)
		{
			if (behind(edges[i].p0, position) != behind(edges[i].p1, position))
			{
				Plane p(edges[i].v0, edges[i].v1, position);
					
				if (behind(p, inside_frustum))
				{
					p = -p;
				}

				polyhedron.add_plane(p);
			}
		}
	}

//	std::cout << polyhedron.planes_.size() << std::endl;

	return polyhedron;
}


Polyhedron Polyhedron::create_directional_shadow_caster_volume(const Frustum& frustum, const float3& direction)
{
	Polyhedron polyhedron;

	// planes facing the light remain
	for (size_t i = 0; i < 6; ++i)
	{
		if (dot(frustum.planes_[i].normal(), direction) < 0.f)
		{
			polyhedron.add_plane(frustum.planes_[i]);
		}
	}

	Edge edges[12];

	// edges with the left frustum plane
	edges[0].p0 = frustum.planes_[0];
	edges[0].p1 = frustum.planes_[2];
	edges[0].v0 = frustum.points_[2]; //left, top, near
	edges[0].v1 = frustum.points_[6]; //left, top, far
			
	edges[1].p0 = frustum.planes_[0];
	edges[1].p1 = frustum.planes_[3];
	edges[1].v0 = frustum.points_[0];	//left, bottom, near
	edges[1].v1 = frustum.points_[4]; //left, bottom, far

	edges[2].p0 = frustum.planes_[0];
	edges[2].p1 = frustum.planes_[4];
	edges[2].v0 = edges[0].v0; //left, near, top
	edges[2].v1 = edges[1].v0; //left, near, bottom

	edges[3].p0 = frustum.planes_[0];
	edges[3].p1 = frustum.planes_[5];
	edges[3].v0 = edges[0].v1; //left, far, top
	edges[3].v1 = edges[1].v1; //left, far, bottom

	//edges with the right frustum plane
	edges[4].p0 = frustum.planes_[1];
	edges[4].p1 = frustum.planes_[2];
	edges[4].v0 = frustum.points_[3];	//right, top, near
	edges[4].v1 = frustum.points_[7]; //right, top, far

	edges[5].p0 = frustum.planes_[1];
	edges[5].p1 = frustum.planes_[3];
	edges[5].v0 = frustum.points_[1]; //right, bottom, near
	edges[5].v1 = frustum.points_[5]; //right, bottom, far

	edges[6].p0 = frustum.planes_[1];
	edges[6].p1 = frustum.planes_[4];
	edges[6].v0 = edges[4].v0; //right, near, top
	edges[6].v1 = edges[5].v0; //right, near, bottom

	edges[7].p0 = frustum.planes_[1];
	edges[7].p1 = frustum.planes_[5];
	edges[7].v0 = edges[4].v1; //right, far, top
	edges[7].v1 = edges[5].v1; //right, far, bottom

	//edges with the top frustum plane
	edges[8].p0 = frustum.planes_[2];
	edges[8].p1 = frustum.planes_[4];
	edges[8].v0 = edges[0].v0; //top, near, left
	edges[8].v1 = edges[4].v0; //top, near, right

	edges[9].p0 = frustum.planes_[2];
	edges[9].p1 = frustum.planes_[5];
	edges[9].v0 = edges[0].v1; //top, far, left
	edges[9].v1 = edges[4].v1; //top, far, right

	//edges with the bottom frustum plane
	edges[10].p0 = frustum.planes_[3];
	edges[10].p1 = frustum.planes_[4];
	edges[10].v0 = edges[1].v0; //bottom, near, left
	edges[10].v1 = edges[5].v0; //bottom, near, right

	edges[11].p0 = frustum.planes_[3];
	edges[11].p1 = frustum.planes_[5];
	edges[11].v0 = edges[1].v1; //bottom, far, left
	edges[11].v1 = edges[5].v1; //bottom, far, right

	float3 inside_frustum = (edges[0].v0 + edges[0].v1 + edges[1].v0 + edges[1].v1
						  +  edges[4].v0 + edges[4].v1 + edges[5].v0 + edges[5].v1) / 8.f;
	
	for (size_t i = 0; i < 12; ++i)
	{
		// one plane faces the light, the other doesn't
		if ((dot(edges[i].p0.normal(), direction) < 0.f) != (dot(edges[i].p1.normal(), direction) < 0.f))
		{
			Plane p(edges[i].v0, edges[i].v1, edges[i].v0 + direction);
				
			if (behind(p, inside_frustum))
			{
				p = -p;
			}

			polyhedron.add_plane(p);
		}
	}

	return polyhedron;
}
