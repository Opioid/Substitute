#pragma once

#include "Vector.hpp"
#include "Matrix.hpp"
#include "Math.hpp"

inline float dot(const Plane& p, const float3& v);
inline Plane transform(const Plane& p, const float4x4& m);

struct Plane
{
	union
	{
		struct
		{
			float a, b, c, d;
		};

		float p[4];
	};

	Plane() {}

	Plane(float a, float b, float c, float d) : a(a), b(b), c(c), d(d)
	{}

	Plane(const float3& normal, float d) : a(normal.x), b(normal.y), c(normal.z), d(d)
	{}

	Plane(const float3& normal, const float3& point) : a(normal.x), b(normal.y), c(normal.z), d(-dot(normal, point))
	{}

	Plane(const float3& v0, const float3& v1, const float3& v2)
	{
		float3 n = normalize(cross(v2 - v1, v0 - v1));
		a = n.x; 
		b = n.y; 
		c = n.z;

		d = -dot(n, v0);
	}
	
	Plane operator-() const { return Plane(-a, -b, -c, -d); }

	inline float sgn(float a) const
	{
		if (a > 0.f) return  1.f;
		if (a < 0.f) return -1.f;

		return 0.f;
	}

	float3 get_normal() const
	{
		return float3(a, b, c);
	}

	float4x4 clipProjectionMatrix(const float4x4& proj, const float4x4& view) const
	{
		//google for: Oblique Frustum Clipping

		// Calculate the clip-space corner point opposite the clipping plane
		// as (sgn(clipPlane.x), sgn(clipPlane.y), 1, 1) and
		// transform it into camera space by multiplying it
		// by the inverse of the projection matrix
	
		Plane tmp = transform(*this, view);

		float4 clipPlane(tmp.a, tmp.b, tmp.c, tmp.d);

		float4 q(float(math::sign(clipPlane.x)) / proj.m00,
				 float(math::sign(clipPlane.y)) / proj.m11,
				 1.f,
				 (1.f - proj.m22) / proj.m32);
	
		// Calculate the scaled plane vector
		float4 c = /*(1.0f / dot(clipPlane, q)) **/ clipPlane / dot(clipPlane, q);
	
		// Replace the third column of the projection matrix
		float4x4 clippedProj = proj;

		clippedProj.m02 = c.x;
		clippedProj.m12 = c.y;
		clippedProj.m22 = c.z;
		clippedProj.m32 = c.w;

		return clippedProj;
	}

	static float3 intersection(const Plane& p0, const Plane& p1, const Plane& p2)
	{
		float3 n1(p0.a, p0.b, p0.c);
		float   d1 = p0.d;
	
		float3 n2(p1.a, p1.b, p1.c);
		float   d2 = p1.d;

		float3 n3(p2.a, p2.b, p2.c);
		float   d3 = p2.d;

	//    d1 ( N2 * N3 ) + d2 ( N3 * N1 ) + d3 ( N1 * N2 )
	//P = ------------------------------------------------
	//                    N1 . ( N2 * N3 ) 
	
		return -(d1 * cross(n2, n3) + d2 * cross(n3, n1) + d3 * cross(n1, n2)) / dot(n1, (cross(n2, n3)));
	}
};

inline float dot(const Plane& p, const float3& v)
{
	return p.a * v.x + p.b * v.y + p.c * v.z + p.d;
}

inline bool behind(const Plane& p, const float3& point)
{
	return dot(p, point) < 0.f;
}

inline Plane normalize(const Plane& p)
{
	float t = 1.f / length(p.get_normal());

	return Plane(p.a * t, p.b * t, p.c * t, p.d * t);
}

inline float3 reflect(const float3& v, const Plane& p)
{
	return -2.f * p.d * p.get_normal() + reflect(v, p.get_normal());
}

inline Plane operator*(const Plane& p, const float4x4& m)
{
	return Plane(p.a * m.m00 + p.b * m.m10 + p.c * m.m20 + p.d * m.m30,
				 p.a * m.m01 + p.b * m.m11 + p.c * m.m21 + p.d * m.m31,
				 p.a * m.m02 + p.b * m.m12 + p.c * m.m22 + p.d * m.m32,
				 p.a * m.m03 + p.b * m.m13 + p.c * m.m23 + p.d * m.m33);
}

inline Plane transform(const Plane& p, const float4x4& m)
{
	float4x4 tmp = transpose(invert(m));

	return p * tmp;
}
