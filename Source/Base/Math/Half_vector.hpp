#pragma once

#include "Vector3.hpp"
#include "half.hpp"

/*
typedef tVector3<half_float::half> half3;
typedef tVector4<half_float::half> half4;
*/

struct half4
{
	union
	{
		struct { half_float::half x, y, z, w; };
		struct { half_float::half r, g, b, a; };
	};

	half4()
	{}

	template<typename T>
	explicit half4(const tVector3<T>& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = T(0);
	}

	template<typename T>
	half4(T _x, T _y, T _z, T _w) 
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	half4 operator+(const half4& v) const
	{
		return half4(x + v.x, y + v.y, z + v.z, w + v.w);
	}

	half4 &operator+=(const half4& v)
	{
		x += v.x; y += v.y; z += v.z; w += v.w;
		return *this;
	}

	half4 &operator/=(float s)
	{
		half_float::half is = half_float::half(1.f / s);
		x *= is; y *= is; z *= is; w *= w;
		return *this;
	}
};

inline half4 operator*(float s, const half4& v)
{
	return half4(s * v.x, s * v.y, s * v.z, s * v.w);
}
