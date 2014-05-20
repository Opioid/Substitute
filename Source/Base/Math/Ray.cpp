#include "Ray.hpp"
#include "Plane.hpp"
#include "Intersection/Sphere.hpp"
#include "Intersection/Cylinder.hpp"
#include <cmath>

Ray3::Ray3()
{}

Ray3::Ray3(const float3& org, const float3& dir) : org(org), dir(dir)
{}

bool Ray3::intersect(const Plane& p, float &dist) const
{
	float numer = dot(p, org);
	float denom = dot(p.get_normal(), dir);

	if (denom == 0.f) return false;

	dist = -(numer / denom);

	return true;
}

bool Ray3::intersect(const Sphere& sphere) const
{
	float3 p = org - sphere.position;
	float a = dot(dir, dir);
	float b = 2.0f * dot(dir, p);
	float c = dot(p, p) - (sphere.radius * sphere.radius);

	float det = (b * b) - (4 * a * c);

	if (det < 0) return false;
	float d = sqrt(det);
//  float t1 = (-b - d) / (2 * a);
	float t2 = (-b + d) / (2 * a);

	if (t2 <= 0) return false;
//  if (t1 < 0) cdist = t2;
//  else cdist = t1;
	if (det == 0) return true;
	return true;
}

bool Ray3::intersect(const Cylinder &cylinder) const
{
	// Create a coordinate system for the cylinder.  In this system, the
	// cylinder segment center C is the origin and the cylinder axis direction
	// W is the z-axis.  U and V are the other coordinate axis directions.
	// If P = x*U+y*V+z*W, the cylinder is x^2 + y^2 = r^2, where r is the
	// cylinder radius.  The end caps are |z| = h/2, where h is the cylinder
	// height.
	float3x3 basis;
	set_basis(basis, cylinder.m_axis);

	float3 U = basis.rows[0];
	float3 V = basis.rows[1];
	float3 W = basis.rows[2];

	float halfHeight = cylinder.m_halfHeight;
	float rSqr = cylinder.m_radius * cylinder.m_radius;

	// convert incoming line origin to cylinder coordinates
	float3 diff = org - cylinder.position;
	float3 P(dot(U, diff), dot(V, diff), dot(W, diff));

	// Get the z-value, in cylinder coordinates, of the incoming line's
	// unit-length direction.
	float dz = dot(W, dir);

	if (abs(dz) >= 1.f)
	{
		// The line is parallel to the cylinder axis.  Determine if the line
		// intersects the cylinder end disks.
		float radialSqrDist = rSqr - P.x*P.x - P.y*P.y;
		if (radialSqrDist < 0.f)
		{
			// Line outside the cylinder, no intersection.
			return false;
		}

		return true;
	}

	// convert incoming line unit-length direction to cylinder coordinates
	float3 D(dot(U, dir), dot(V, dir), dz);

	float a0, a1, a2, discr, root, inv, tValue;

	if (abs(D.z) <= 0.f)
	{
		// The line is perpendicular to the cylinder axis.
		if (abs(P.z) > halfHeight)
		{
			// Line is outside the planes of the cylinder end disks.
			return false;
		}

		// Test intersection of line P+t*D with infinite cylinder
		// x^2+y^2 = r^2.  This reduces to computing the roots of a
		// quadratic equation.  If P = (P.x,py,pz) and D = (D.x,dy,dz),
		// then the quadratic equation is
		//   (D.x^2+dy^2)*t^2 + 2*(P.x*D.x+py*dy)*t + (P.x^2+py^2-r^2) = 0
		a0 = P.x*P.x + P.y*P.y - rSqr;
		a1 = P.x*D.x + P.y*D.y;
		a2 = D.x*D.x + D.y*D.y;
		discr = a1*a1 - a0*a2;

		if (discr < 0.f)
		{
			// Line does not intersect cylinder.
			return false;
		}
		else
		{
			// Line is tangent to the cylinder.
			return true;
		}
	}

	// Test plane intersections first.
	inv = ((float)1.0)/D.z;

	float t0 = (-halfHeight - P.z)*inv;
	float xTmp = P.x + t0*D.x;
	float yTmp = P.y + t0*D.y;
	if (xTmp*xTmp + yTmp*yTmp <= rSqr)
	{
		// Planar intersection inside the top cylinder end disk.
		return true;
	}

	float t1 = (+halfHeight - P.z)*inv;
	xTmp = P.x + t1*D.x;
	yTmp = P.y + t1*D.y;
	if (xTmp*xTmp + yTmp*yTmp <= rSqr)
	{
		// Planar intersection inside the bottom cylinder end disk.
		return true;
	}

	return true;
}

#define EPSILON 0.000001

bool Ray3::intersectTriangle(const float3& v0, const float3& v1, const float3& v2, float &t) const
{
/*int
intersect_triangle(double orig[3], double dir[3],
				   double vert0[3], double vert1[3], double vert2[3],
				   double *t, double *u, double *v)*/
//{
   //double edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
	float3 edge1, edge2, tvec, pvec, qvec;
	double det,inv_det , u, v;

   /* find vectors for two edges sharing vert0 */
   //SUB(edge1, vert1, vert0);
   //SUB(edge2, vert2, vert0);
	edge1 = v1 - v0;
	edge2 = v2 - v0;

   /* begin calculating determinant - also used to calculate U parameter */
   //CROSS(pvec, dir, edge2);
	pvec = cross(dir, edge2);

   /* if determinant is near zero, ray lies in plane of triangle */
   //det = DOT(edge1, pvec);
	det = dot(edge1, pvec);

   if (det > -EPSILON && det < EPSILON)
	 return false;
   inv_det = 1.0 / det;

   /* calculate distance from vert0 to ray origin */
   //SUB(tvec, orig, vert0);
   tvec = org - v0;

   /* calculate U parameter and test bounds */
   //*u = DOT(tvec, pvec) * inv_det;
   u = dot(tvec, pvec) * inv_det;
   if (u < 0.0 || u > 1.0)
	 return false;

   /* prepare to test V parameter */
   //CROSS(qvec, tvec, edge1);
   qvec = cross(tvec, edge1);

   /* calculate V parameter and test bounds */
   //*v = DOT(dir, qvec) * inv_det;
   v = dot(dir, qvec) * inv_det;
   if (v < 0.0 || u + v > 1.0)
	 return false;

   /* calculate t, ray intersects triangle */
   t = dot(edge2, qvec) * float(inv_det);

   return true;
//}
}

bool Ray3::intersectQuad(const float3& v0, const float3& v1, const float3& v2, const float3& v3, float &t) const
{
	return intersectTriangle(v0, v1, v2, t) || intersectTriangle(v0, v2, v3, t);
}
