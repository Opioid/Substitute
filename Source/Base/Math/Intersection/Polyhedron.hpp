#pragma once

#include "Frustum.hpp"
#include <vector>

class Polyhedron
{

public:

	void add_plane(const Plane& plane);

	Intersection_type intersect(const AABB& aabb) const;
	Intersection_type intersect(const AABB& aabb, size_t &out) const;

	static Polyhedron create_point_shadow_caster_volume(const Frustum& frustum, const float3& position);
	static Polyhedron create_directional_shadow_caster_volume(const Frustum& frustum, const float3& direction);

	std::vector<Plane> planes_;

private:

	struct Edge
	{
		Plane p0;
		Plane p1;
		float3 v0;
		float3 v1;
	};
};
