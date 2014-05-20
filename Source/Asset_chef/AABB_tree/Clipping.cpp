#include "Clipping.hpp"
#include "Math/Plane.hpp"


#define EPSILON	 1.0e-8f


bool triangle_behind_plane(const Triangle &tri, const Plane &plane)
{
	if      (dot(plane, tri.v[0].position) <= -0.00001f) return true;
	else if (dot(plane, tri.v[1].position) <= -0.00001f) return true;
	else if (dot(plane, tri.v[2].position) <= -0.00001f) return true;

	else return false;
}


bool triangle_intersect_plane(const Triangle &tri, const Plane &plane)
{
	size_t positive = 0;
	size_t negative = 0;

	if      (dot(plane, tri.v[0].position) >  EPSILON) ++positive;
	else if (dot(plane, tri.v[0].position) < -EPSILON) ++negative;

	if      (dot(plane, tri.v[1].position) >  EPSILON) ++positive;
	else if (dot(plane, tri.v[1].position) < -EPSILON) ++negative;

	if      (dot(plane, tri.v[2].position) >  EPSILON) ++positive;
	else if (dot(plane, tri.v[2].position) < -EPSILON) ++negative;

	return positive && negative;
}


void lineIntersection(const rendering::Vertex_position3x32_tex_coord2x32_normal3x32 &start, const rendering::Vertex_position3x32_tex_coord2x32_normal3x32 &end, rendering::Vertex_position3x32_tex_coord2x32_normal3x32& vertexOut, const Plane& plane)
{
	float start_dist = std::abs(dot(plane, start.position));
	float end_dist   = std::abs(dot(plane, end.position));

	float start_kvot = std::abs(start_dist) / (std::abs(start_dist) + std::abs(end_dist));

	float3 delta = end.position - start.position;
	vertexOut.position = start.position + start_kvot * delta;

	float2 deltaTex = end.tex_coord - start.tex_coord;
	vertexOut.tex_coord = start.tex_coord + start_kvot * deltaTex;

	float3 deltaNorm = end.normal - start.normal;
	vertexOut.normal = normalize(start.normal + start_kvot * deltaNorm);
}


/*
bool clipTriangle(std::vector<Triangle> &result, const Triangle &tri, const Plane &plane)
{
	std::vector<const Vertex_textured_ntb*> positive, negative;
	positive.reserve(3); negative.reserve(3);

	const Vertex_textured_ntb *onPlane = 0;

	if      (dot(plane, tri.v[0].position) >  EPSILON) positive.push_back(&tri.v[0]);
	else if (dot(plane, tri.v[0].position) < -EPSILON) negative.push_back(&tri.v[0]);
	else                                            onPlane = &tri.v[0];

	if      (dot(plane, tri.v[1].position) >  EPSILON) positive.push_back(&tri.v[1]);
	else if (dot(plane, tri.v[1].position) < -EPSILON) negative.push_back(&tri.v[1]);
	else                                            onPlane = &tri.v[1];

	if      (dot(plane, tri.v[2].position) >  EPSILON) positive.push_back(&tri.v[2]);
	else if (dot(plane, tri.v[2].position) < -EPSILON) negative.push_back(&tri.v[2]);
	else                                            onPlane = &tri.v[2];



	if (!(negative.size() && positive.size()))		//no need to clip
	{
		if (negative.size()) result.push_back(tri);

		return false;
	}
	else if (onPlane)
	{
		Vertex_textured_ntb shared;
		lineIntersection(*negative[0], *positive[0], shared, plane);

		Triangle tri0(*negative[0], shared, *onPlane);
		Triangle tri1(*positive[0], shared, *onPlane);

		if (triangleBehindPlane(tri0, plane)) result.push_back(tri0);
		if (triangleBehindPlane(tri1, plane)) result.push_back(tri1);

		return true;
	}
	else
	{
		Vertex_textured_ntb shared[2];

		const Vertex_textured_ntb *triangle0[4];
		const Vertex_textured_ntb *triangle1[3];

		if (negative.size() > positive.size())
		{
			lineIntersection(*negative[0], *positive[0], shared[0], plane);
			lineIntersection(*negative[1], *positive[0], shared[1], plane);

			triangle0[0] = &shared[0];
			triangle0[1] = &shared[1];
			triangle0[2] = negative[0];
			triangle0[3] = negative[1];

			triangle1[0] = &shared[0];
			triangle1[1] = &shared[1];
			triangle1[2] = positive[0];
		}
		else
		{
			lineIntersection(*positive[0], *negative[0], shared[0], plane);
			lineIntersection(*positive[1], *negative[0], shared[1], plane);

			triangle0[0] = &shared[0];
			triangle0[1] = &shared[1];
			triangle0[2] = positive[0];
			triangle0[3] = positive[1];

			triangle1[0] = &shared[0];
			triangle1[1] = &shared[1];
			triangle1[2] = negative[0];
		}

 
		Triangle tri0(*triangle0[0], *triangle0[1], *triangle0[2]);
		Triangle tri1(*triangle0[1], *triangle0[2], *triangle0[3]);
		Triangle tri2(*triangle1[0], *triangle1[1], *triangle1[2]);

		if (triangleBehindPlane(tri0, plane)) result.push_back(tri0);
		if (triangleBehindPlane(tri1, plane)) result.push_back(tri1);
		if (triangleBehindPlane(tri2, plane)) result.push_back(tri2);

		return true;
	}
}*/


bool clipTriangle(std::vector<Triangle> &result, const Triangle &tri, const Plane &plane, std::vector<const rendering::Vertex_position3x32_tex_coord2x32_normal3x32*> &positive, std::vector<const rendering::Vertex_position3x32_tex_coord2x32_normal3x32*> &negative)
{
	const rendering::Vertex_position3x32_tex_coord2x32_normal3x32 *onPlane = nullptr;

	if      (dot(plane, tri.v[0].position) >  EPSILON) positive.push_back(&tri.v[0]);
	else if (dot(plane, tri.v[0].position) < -EPSILON) negative.push_back(&tri.v[0]);
	else                                            onPlane = &tri.v[0];

	if      (dot(plane, tri.v[1].position) >  EPSILON) positive.push_back(&tri.v[1]);
	else if (dot(plane, tri.v[1].position) < -EPSILON) negative.push_back(&tri.v[1]);
	else                                            onPlane = &tri.v[1];

	if      (dot(plane, tri.v[2].position) >  EPSILON) positive.push_back(&tri.v[2]);
	else if (dot(plane, tri.v[2].position) < -EPSILON) negative.push_back(&tri.v[2]);
	else                                            onPlane = &tri.v[2];



	if (!(negative.size() && positive.size()))		//no need to clip
	{
		if (negative.size()) result.push_back(tri);

		return false;
	}
	else if (onPlane)
	{
		rendering::Vertex_position3x32_tex_coord2x32_normal3x32 shared;
		lineIntersection(*negative[0], *positive[0], shared, plane);

		Triangle tri0(*negative[0], shared, *onPlane);
		Triangle tri1(*positive[0], shared, *onPlane);

		if (triangle_behind_plane(tri0, plane)) result.push_back(tri0);
		if (triangle_behind_plane(tri1, plane)) result.push_back(tri1);

		return true;
	}
	else
	{
		rendering::Vertex_position3x32_tex_coord2x32_normal3x32 shared[2];

		const rendering::Vertex_position3x32_tex_coord2x32_normal3x32 *triangle0[4];
		const rendering::Vertex_position3x32_tex_coord2x32_normal3x32 *triangle1[3];

		if (negative.size() > positive.size())
		{
			lineIntersection(*negative[0], *positive[0], shared[0], plane);
			lineIntersection(*negative[1], *positive[0], shared[1], plane);

			triangle0[0] = &shared[0];
			triangle0[1] = &shared[1];
			triangle0[2] = negative[0];
			triangle0[3] = negative[1];

			triangle1[0] = &shared[0];
			triangle1[1] = &shared[1];
			triangle1[2] = positive[0];
		}
		else
		{
			lineIntersection(*positive[0], *negative[0], shared[0], plane);
			lineIntersection(*positive[1], *negative[0], shared[1], plane);

			triangle0[0] = &shared[0];
			triangle0[1] = &shared[1];
			triangle0[2] = positive[0];
			triangle0[3] = positive[1];

			triangle1[0] = &shared[0];
			triangle1[1] = &shared[1];
			triangle1[2] = negative[0];
		}

 
		Triangle tri0(*triangle0[0], *triangle0[1], *triangle0[2]);
		Triangle tri1(*triangle0[1], *triangle0[2], *triangle0[3]);
		Triangle tri2(*triangle1[0], *triangle1[1], *triangle1[2]);

		if (triangle_behind_plane(tri0, plane)) result.push_back(tri0);
		if (triangle_behind_plane(tri1, plane)) result.push_back(tri1);
		if (triangle_behind_plane(tri2, plane)) result.push_back(tri2);

		return true;
	}
}
