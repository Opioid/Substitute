#pragma once

#include "Vector2.hpp"
#include "Simd_intrinsics.hpp"
#include <cmath>
#include <algorithm>
#include <cstdint>

#undef min
#undef max

template<typename T>
struct tVector3
{
	union
	{
		struct { T x, y, z; };
		struct { T r, g, b; };

		struct { tVector2<T> xy; T _z; };

		T v[3];
	};

	tVector3()
	{}

	tVector3(T x, T y, T z) : x(x), y(y), z(z)
	{}

	explicit tVector3(const tVector2<T>& xy, T z = T(0)) : xy(xy), _z(z)
	{}

	explicit tVector3(const T* v) : x(v[0]), y(v[1]), z(v[2])
	{}

	tVector3 operator+(const tVector3& v) const
	{
		return tVector3(x + v.x, y + v.y, z + v.z);
	}

	tVector3 operator-(const tVector3& v) const
	{
		return tVector3(x - v.x, y - v.y, z - v.z);
	}
		
	tVector3 operator*(const tVector3& v) const
	{
		return tVector3(x * v.x, y * v.y, z * v.z);
	}
		
	tVector3 operator/(T s) const
	{
		T is = T(1) / s;
		return tVector3(is * x, is * y, is * z);
	}

	tVector3 operator-() const
	{
		return tVector3(-x, -y, -z);
	}

	tVector3 &operator+=(const tVector3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	tVector3 &operator-=(const tVector3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	tVector3 &operator*=(const tVector3& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	tVector3 &operator*=(T s)
	{
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	tVector3 &operator/=(T s)
	{
		T is = T(1) / s;
		x *= is;
		y *= is;
		z *= is;
		return *this;
	}

	bool operator==(const tVector3& v) const
	{
		return x == v.x && y == v.y && z == v.z;
	}

	bool operator!=(const tVector3& v) const
	{
		return x != v.x || y != v.y || z != v.z;
	}

	explicit operator unsigned int() const
	{
	/*	const __m128 m4x255f = _mm_set_ps1(255.f);

		__m128 m0 = _mm_set_ps(b, g, r, 0.f);
		
		m0 = _mm_mul_ps(m0, m4x255f);

		__m128i m1 = _mm_cvtps_epi32(m0);

		if (m1.m128i_i32[3] > 255) m1.m128i_i32[3] = 255;
		if (m1.m128i_i32[2] > 255) m1.m128i_i32[2] = 255;
		if (m1.m128i_i32[1] > 255) m1.m128i_i32[1] = 255;

		return 0xff000000 | (m1.m128i_i32[3] << 16) | (m1.m128i_i32[2] << 8) | m1.m128i_i32[1];
		*/

		unsigned int red  (r * T(255));
		unsigned int green(g * T(255));
		unsigned int blue (b * T(255));

		return 0xff000000 | (blue << 16) | (green << 8) | red;
	}

	T absolute_max(uint32_t& i) const
	{
		T ax = std::abs(x);
		T ay = std::abs(y);
		T az = std::abs(z);

		if (ax >= ay && ax >= az)
		{
			i = 0;
			return ax;
		}
		
		if (ay >= ax && ay >= az)
		{
			i = 1;
			return ay;
		}

		i = 2;
		return az;
	}

	static const tVector3 identity;
};

template<typename T>
const tVector3<T> tVector3<T>::identity(T(0), T(0), T(0));

template<typename T>
inline tVector3<T> operator*(T s, const tVector3<T>& v)
{
	return tVector3<T>(s * v.x, s * v.y, s * v.z);
}

template<typename T>
inline T dot(const tVector3<T> &a, const tVector3<T> &b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

template<typename T>
inline T length(const tVector3<T>& v)
{
	return sqrt(dot(v, v));
}

template<typename T>
inline T squared_length(const tVector3<T>& v)
{
	return dot(v, v);
}

template<typename T>
inline tVector3<T> normalize(const tVector3<T>& v)
{
	return v / length(v);
}

template<typename T>
inline tVector3<T> reciprocal(const tVector3<T>& v)
{
	return tVector3<T>(1.f / v.x, 1.f / v.y, 1.f / v.z);
}

template<typename T>
inline tVector3<T> absolute(const tVector3<T>& v)
{
	return tVector3<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z));
}

template<typename T>
inline tVector3<T> cross(const tVector3<T> &a, const tVector3<T> &b)
{
	return tVector3<T>(a.y * b.z - a.z * b.y,
					   a.z * b.x - a.x * b.z,
					   a.x * b.y - a.y * b.x);
}

template<typename T>
inline tVector3<T> project(const tVector3<T> &a, const tVector3<T> &b)
{
	return dot(b, a) * b;
}

template<typename T>
inline T distance(const tVector3<T> &a, const tVector3<T> &b)
{
	return length(a - b);
}

template<typename T>
inline T squared_distance(const tVector3<T> &a, const tVector3<T> &b)
{
	return squared_length(a - b);
}

template<typename T>
inline tVector3<T> saturate(const tVector3<T>& v)
{
	return tVector3<T>(std::min(std::max(v.x, T(0)), T(1)), std::min(std::max(v.y, T(0)), T(1)), std::min(std::max(v.z, T(0)), T(1)));
}

template<typename T>
inline tVector3<T> lerp(const tVector3<T> &a, const tVector3<T> &b, T t)
{
	T u = T(1) - t;
	return u * a + t * b;//tVector3<T>(a.x * u + b.x * t, a.y * u + b.y * t, a.z * u + b.z * t);
}

template<typename T>
inline tVector3<T> reflect(const tVector3<T>& v, const tVector3<T> &normal)
{
	return v - 2.f * dot(v, normal) * normal;
}

namespace math
{

inline tVector3<float> min(const tVector3<float>& a, const tVector3<float>& b)
{
	/*
	__m128 ma = _mm_set_ps(a.x, a.y, a.z, 0.f);
	__m128 mb = _mm_set_ps(b.x, b.y, b.z, 0.f);

	__m128 mm =_mm_min_ps(ma, mb);

	return tVector3<float>(mm.m128_f32[3], mm.m128_f32[2], mm.m128_f32[1]);
*/
	return tVector3<float>(a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y, a.z < b.z ? a.z : b.z);
}

inline tVector3<float> max(const tVector3<float>& a, const tVector3<float>& b)
{
	/*
	__m128 ma = _mm_set_ps(a.x, a.y, a.z, 0.f);
	__m128 mb = _mm_set_ps(b.x, b.y, b.z, 0.f);

	__m128 mm =_mm_max_ps(ma, mb);

	return tVector3<float>(mm.m128_f32[3], mm.m128_f32[2], mm.m128_f32[1]);
*/
	return tVector3<float>(a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y, a.z > b.z ? a.z : b.z);
}

}
