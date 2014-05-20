#pragma once

#include <cmath>

template<typename T, int num_bits>
struct Normalized_unsigned_fixed_point
{
	T v;

	Normalized_unsigned_fixed_point(T v) : v(v)
	{}

	Normalized_unsigned_fixed_point(float x)
	{
		x *= s_factor;

		v = T(floor(x + 0.5f));
	}

	operator T() const
	{
		return v;
	}

	operator float() const
	{
		return float(v) / s_factor;
	}

	static const float s_factor;
};

template<typename T, int num_bits>
const float Normalized_unsigned_fixed_point<T, num_bits>::s_factor(pow(2.f, num_bits) - 1.f);

template<typename T, int num_bits>
struct Normalized_fixed_point
{
	T v;

	Normalized_fixed_point(T v) : v(v)
	{}

	Normalized_fixed_point(float x)
	{
		x *= s_factor;

		v = T(floor(x + 0.5f));
	}

	operator T() const
	{
		return v;
	}

	operator float() const
	{
		return float(v) / s_factor;
	}

	static const float s_factor;
};

template<typename T, int num_bits>
const float Normalized_fixed_point<T, num_bits>::s_factor(pow(2.f, num_bits - 1) - 1.f);


typedef Normalized_unsigned_fixed_point<unsigned char,   2> UNorm2;
typedef Normalized_unsigned_fixed_point<unsigned char,   8> UNorm8;
typedef Normalized_unsigned_fixed_point<unsigned short, 10> UNorm10;

typedef Normalized_fixed_point<char,   2> SNorm2;
typedef Normalized_fixed_point<short, 10> SNorm10;
typedef Normalized_fixed_point<short, 16> SNorm16;

/*
struct Unorm10
{
	//1023 == 2 ^ (10) - 1
	static unsigned short convert(float x)
	{
		x *= 1023.f; 
		return unsigned short(floor(x + 0.5f));
	}

	static float convert(unsigned short x)
	{
		return float(x) / 1023.f;
	}
};


struct Snorm16
{
	//32767 == 2 ^ (16 - 1) - 1
	static short convert(float x)
	{
		x *= 32767.f; 
		return short(floor(x + 0.5f));
	}

	static float convert(short x)
	{
		return float(x) / 32767.f; 
	}
};
*/
