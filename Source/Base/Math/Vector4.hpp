#pragma once

#include "Vector3.hpp"

template<typename T>
struct Vector4
{
	union
	{
		struct
		{
			T x;
			T y;
			T z;
			T w;
		};

		struct
		{
			T r;
			T g;
			T b;
			T a;
		};

		struct
		{
			Vector2<T> xy;
			Vector2<T> zw;
		};

		struct
		{
			Vector3<T> xyz;
			T _w;
		};

		struct
		{
			Vector3<T> rgb;
			T _a;
		};

		T v[4];
	};

	Vector4();

	Vector4(T x, T y, T z, T w = T(1));

	explicit Vector4(const Vector2<T>& xy, T z, T w = T(1));

	Vector4(const Vector2<T>& xy, const Vector2<T>& zw);

	explicit Vector4(const Vector3<T>& xyz, T w = T(1));

	Vector4 operator*(const Vector4<T>& v) const;

	Vector4 operator/(T s) const;

	Vector4& operator+=(const Vector4& v);

	Vector4& operator-=(const Vector4& v);

	explicit operator uint32_t() const;

	static const Vector4 identity;
};

template<typename T>
Vector4<T> operator*(T s, const Vector4<T> &v);

template<typename T>
T dot(const Vector4<T>& a, const Vector4<T>& b);
