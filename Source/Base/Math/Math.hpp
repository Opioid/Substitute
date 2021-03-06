#pragma once

#include "Vector.hpp"
#include <cmath>
#include <cstdlib>
#include <algorithm>

namespace math
{

extern const float pi;
extern const float pi_mul_2;
extern const float pi_div_2;
extern const float pi_div_180;
extern const float _180_div_pi;

inline float to_radians(float degrees)
{
	return degrees * pi_div_180;
}

inline float to_degrees(float radians)
{
	return radians * _180_div_pi;
}

template <typename T>
inline T cot(T x)
{
	return tan(pi_div_2 - x);
}

inline int random(int min, int max)
{
	++max;

	return min + (max - min) * ::rand() / int(RAND_MAX);
}

inline float random(float min, float max)
{
	return min + (max - min) * ::rand() / float(RAND_MAX);
}
	
float3 random_disk();
float3 random_sphere();

template <typename T>
inline int sign(T val)
{
	return (T(0) < val) - (val < T(0));
}

template<typename T>
inline T frac(T x)
{
	return x - std::floor(x);
}

template<typename T>
inline T round(T x)
{
	return std::floor(x >= T(0) ? x + T(0.5) : x - T(0.5));
}

template<typename T>
inline T saturate(T a)
{
	return std::min(std::max(a, T(0)), T(1));
}

template<typename T>
inline T lerp(T a, T b, T t)
{
	T u = T(1) - t;
	return u * a + t * b;
}

template<typename T>
inline T exp2(T x)
{
	return pow(T(2), x);
}

template<typename T>
inline T inverse_sqrt(T x)
{
	return T(1) / std::sqrt(x);
}

}
