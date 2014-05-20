#pragma once

#include <cmath>

template<typename T>
struct tVector2
{
	union
	{
		struct { T x, y; };

		T v[2];
	};

	tVector2()
	{}

	tVector2(T x, T y) : x(x), y(y)
	{}

	template<typename U>
	explicit tVector2(const tVector2<U> &v) : x(T(v.x)), y(T(v.y))
	{}

	tVector2 operator+(const tVector2& v) const
	{
		return tVector2(x + v.x, y + v.y);
	}

	tVector2 operator-(const tVector2& v) const
	{
		return tVector2(x - v.x, y - v.y);
	}

	tVector2 operator*(const tVector2& v) const
	{
		return tVector2(x * v.x, y * v.y);
	}

	tVector2 operator/(T s) const
	{
		return tVector2(x / s, y / s);
	}

	tVector2 &operator+=(const tVector2& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	tVector2 &operator/=(T s)
	{
		x /= s;
		y /= s;
		return *this;
	}

	bool operator==(const tVector2& v) const
	{
		return x == v.x && y == v.y;
	}

	bool operator!=(const tVector2& v) const
	{
		return x != v.x || y != v.y;
	}

	static const tVector2 identity;
};

template<typename T>
const tVector2<T> tVector2<T>::identity(T(0), T(0));

template<typename T>
inline tVector2<T> operator*(T s, const tVector2<T>& v)
{
	return tVector2<T>(s * v.x, s * v.y);
}

template<typename T>
inline tVector2<T> operator/(T s, const tVector2<T>& v)
{
	return tVector2<T>(s / v.x, s / v.y);
}

template<typename T>
inline T dot(const tVector2<T>& a, const tVector2<T>& b)
{
	return a.x * b.x + a.y * b.y;
}

template<typename T>
inline T length(const tVector2<T>& v)
{
	return sqrt(dot(v, v));
}

template<typename T>
inline T squared_length(const tVector2<T>& v)
{
	return dot(v, v);
}

template<typename T>
inline tVector2<T> normalize(const tVector2<T>& v)
{
	return v / length(v);
}

template<typename T>
inline tVector2<T> reciprocal(const tVector2<T>& v)
{
	return tVector2<T>(1.f / v.x, 1.f / v.y);
}

template<typename T>
inline tVector2<T> round(const tVector2<T>& v)
{
	return tVector2<T>(std::floor(v.x >= T(0) ? v.x + T(0.5) : v.x - T(0.5)), std::floor(v.y >= T(0) ? v.y + T(0.5) : v.y - T(0.5)));
}
