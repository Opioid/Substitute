#pragma once

#include "Vector3.hpp"

template<typename T>
struct tVector4
{
	union
	{
		struct { T x, y, z, w; };
		struct { T r, g, b, a; };

		struct { tVector2<T> xy, zw; };

		struct { tVector3<T> xyz; T _w; };
		struct { tVector3<T> rgb; T _a; };

		T v[4];
	};

	tVector4()
	{}

	tVector4(T x, T y, T z, T w = 1.f) : x(x), y(y), z(z), w(w)
	{}

	explicit tVector4(const tVector2<T> &xy, T z, T w = T(1)) : xy(xy), zw(z, w)
	{}

	tVector4(const tVector2<T> &xy, const tVector2<T> &zw) : xy(xy), zw(zw)
	{}

	explicit tVector4(const tVector3<T>& xyz, T w = T(1)) : xyz(xyz), _w(w)
	{}

	tVector4 operator*(const tVector4<T>& v) const
	{
		return tVector4(x * v.x, y * v.y, z * v.z, w * v.w);
	}

	tVector4 operator/(T s) const
	{
		T is = T(1) / s;
		return tVector4(is * x, is * y, is * z, is * w);
	}

	tVector4& operator+=(const tVector4& v)
	{
		x += v.x; y += v.y; z += v.z; w += v.w;
		return *this;
	}

	tVector4& operator-=(const tVector4& v)
	{
		x -= v.x; y -= v.y; z -= v.z; w -= v.w;
		return *this;
	}

    explicit operator unsigned int() const
	{
		/*
		const __m128 m4x255f = _mm_set_ps1(255.f);

		__m128 m0 = _mm_set_ps(r, g, b, a);

		m0 = _mm_mul_ps(m0, m4x255f);

		__m128i m1 = _mm_cvtps_epi32(m0);

		if (m1.m128i_i32[3] > 255)
		{
			m1.m128i_i32[3] = 255;
		}

		if (m1.m128i_i32[2] > 255)
		{
			m1.m128i_i32[2] = 255;
		}

		if (m1.m128i_i32[1] > 255)
		{
			m1.m128i_i32[1] = 255;
		}

		if (m1.m128i_i32[0] > 255)
		{
			m1.m128i_i32[0] = 255;
		}

		return  (m1.m128i_i32[0] << 24) | (m1.m128i_i32[1] << 16) | (m1.m128i_i32[2] << 8) | m1.m128i_i32[3];
		*/

		unsigned int red  (r * T(255));
		unsigned int green(g * T(255));
		unsigned int blue (b * T(255));
		unsigned int alpha(a * T(255));

		return  (alpha << 24) | (blue << 16) | (green << 8) | red;
	}

	static const tVector4 identity;
};

template<typename T>
const tVector4<T> tVector4<T>::identity(T(0), T(0), T(0), T(0));

template<typename T>
inline tVector4<T> operator*(T s, const tVector4<T> &v)
{
	return tVector4<T>(s * v.x, s * v.y, s * v.z, s * v.w);
}

template<typename T>
inline T dot(const tVector4<T>& a, const tVector4<T>& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}
