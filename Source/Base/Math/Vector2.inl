#pragma once

#include "Vector2.hpp"

template<typename T>
Vector2<T>::Vector2()
{}

template<typename T>
Vector2<T>::Vector2(T x, T y) : x(x), y(y)
{}

template<typename T>
template<typename U>
Vector2<T>::Vector2(const Vector2<U>& v) : x(T(v.x)), y(T(v.y))
{}

template<typename T>
Vector2<T> Vector2<T>::operator+(const Vector2& v) const
{
	return Vector2(x + v.x, y + v.y);
}

template<typename T>
Vector2<T> Vector2<T>::operator-(const Vector2& v) const
{
	return Vector2(x - v.x, y - v.y);
}

template<typename T>
Vector2<T> Vector2<T>::operator*(const Vector2& v) const
{
	return Vector2(x * v.x, y * v.y);
}

template<typename T>
Vector2<T> Vector2<T>::operator/(T s) const
{
	return Vector2(x / s, y / s);
}

template<typename T>
Vector2<T>& Vector2<T>::operator+=(const Vector2& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

template<typename T>
Vector2<T>& Vector2<T>::operator/=(T s)
{
	x /= s;
	y /= s;
	return *this;
}

template<typename T>
bool Vector2<T>::operator==(const Vector2& v) const
{
	return x == v.x && y == v.y;
}

template<typename T>
bool Vector2<T>::operator!=(const Vector2& v) const
{
	return x != v.x || y != v.y;
}

template<typename T>
const Vector2<T> Vector2<T>::identity(T(0), T(0));

template<typename T>
inline Vector2<T> operator*(T s, const Vector2<T>& v)
{
	return Vector2<T>(s * v.x, s * v.y);
}

template<typename T>
inline Vector2<T> operator/(T s, const Vector2<T>& v)
{
	return Vector2<T>(s / v.x, s / v.y);
}

template<typename T>
inline T dot(const Vector2<T>& a, const Vector2<T>& b)
{
	return a.x * b.x + a.y * b.y;
}

template<typename T>
inline T length(const Vector2<T>& v)
{
	return std::sqrt(dot(v, v));
}

template<typename T>
inline T squared_length(const Vector2<T>& v)
{
	return dot(v, v);
}

template<typename T>
inline Vector2<T> normalize(const Vector2<T>& v)
{
	return v / length(v);
}

template<typename T>
inline Vector2<T> reciprocal(const Vector2<T>& v)
{
	return Vector2<T>(1.f / v.x, 1.f / v.y);
}

template<typename T>
inline Vector2<T> round(const Vector2<T>& v)
{
	return Vector2<T>(std::floor(v.x >= T(0) ? v.x + T(0.5) : v.x - T(0.5)), std::floor(v.y >= T(0) ? v.y + T(0.5) : v.y - T(0.5)));
}
