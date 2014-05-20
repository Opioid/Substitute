#pragma once

#include "Normalized_fixed_point.hpp"
#include "Vector.hpp"
#include <cstdint>

struct Vector4_4xUNorm8
{
	unsigned char x, y, z, w;

	Vector4_4xUNorm8()
	{}

	Vector4_4xUNorm8(float4 const& v) : x(UNorm8(v.x)), y(UNorm8(v.y)), z(UNorm8(v.z)), w(UNorm8(v.w))
	{}
};

struct Vector4_UNorm1010102
{
	uint32_t v;

	Vector4_UNorm1010102()
	{}

	Vector4_UNorm1010102(uint32_t v) : v(v)
	{}

	Vector4_UNorm1010102(float x, float y, float z, float w)
	{
		unsigned char  cw = UNorm2 ((w + 1.f) * 0.5f);
		unsigned short cz = UNorm10((z + 1.f) * 0.5f);
		unsigned short cy = UNorm10((y + 1.f) * 0.5f);
		unsigned short cx = UNorm10((x + 1.f) * 0.5f);

		v = cw << 30 | cz << 20 | cy << 10 | cx;
	}

	Vector4_UNorm1010102(const float3& vec)
	{
		unsigned char  cw = UNorm2 (  (0.f + 1.f) * 0.5f);
		unsigned short cz = UNorm10((vec.z + 1.f) * 0.5f);
		unsigned short cy = UNorm10((vec.y + 1.f) * 0.5f);
		unsigned short cx = UNorm10((vec.x + 1.f) * 0.5f);

		v = cw << 30 | cz << 20 | cy << 10 | cx;

		/*
		static const __m128 m1  = _mm_load_ps(one);
		static const __m128 m05 = _mm_set_ps1(0.5f);
		static const __m128 mf  = _mm_set_ps(UNorm2::s_factor, UNorm10::s_factor, UNorm10::s_factor, UNorm10::s_factor);

		__m128 mv = _mm_set_ps(0.f, vec.z, vec.y, vec.x);

		mv = _mm_add_ps(mv, m1);
		mv = _mm_mul_ps(mv, m05);
		mv = _mm_mul_ps(mv, mf);
		mv = _mm_add_ps(mv, m05);
		mv = _mm_door_ps(mv);

		__m128i mi = _mm_cvtps_epi32(mv);

		v = mi.m128i_i32[3] << 30 | mi.m128i_i32[2] << 20 | mi.m128i_i32[1] << 10 | mi.m128i_i32[0];
		*/
	}

	Vector4_UNorm1010102(const float3& vec, float w)
	{
		unsigned char  cw = UNorm2 (    (w + 1.f) * 0.5f);
		unsigned short cz = UNorm10((vec.z + 1.f) * 0.5f);
		unsigned short cy = UNorm10((vec.y + 1.f) * 0.5f);
		unsigned short cx = UNorm10((vec.x + 1.f) * 0.5f);

		v = cw << 30 | cz << 20 | cy << 10 | cx;
	}

	Vector4_UNorm1010102(float4 const& vec) 
	{
		unsigned char  cw = UNorm2 ((vec.w + 1.f) * 0.5f);
		unsigned short cz = UNorm10((vec.z + 1.f) * 0.5f);
		unsigned short cy = UNorm10((vec.y + 1.f) * 0.5f);
		unsigned short cx = UNorm10((vec.x + 1.f) * 0.5f);

		v = cw << 30 | cz << 20 | cy << 10 | cx;
	}

	static Vector4_UNorm1010102 withUnsignedW(const float3& vec, float w)
	{
		unsigned char  cw = UNorm2(w);
		unsigned short cz = UNorm10((vec.z + 1.f) * 0.5f);
		unsigned short cy = UNorm10((vec.y + 1.f) * 0.5f);
		unsigned short cx = UNorm10((vec.x + 1.f) * 0.5f);

		return Vector4_UNorm1010102(cw << 30 | cz << 20 | cy << 10 | cx);
	}

	static const unsigned short s_mask = 0x3ff;

	float get_x() const
	{
		unsigned short us(v);
		us = us & s_mask;

		UNorm10 unorm10(us);
		return 2.f * (float(UNorm10(us)) - 0.5f);

	//	return 2.f * (float(UNorm10(unsigned short(unsigned short(v) & s_mask))) - 0.5f);
	}

	float get_y() const
	{
		unsigned short us(v >> 10);
		us = us & s_mask;

		return 2.f * (float(UNorm10(us)) - 0.5f);

	//	return 2.f * (float(UNorm10(unsigned short(unsigned short(v >> 10) & s_mask))) - 0.5f);
	}

	float get_z() const
	{
		unsigned short us(v >> 20);
		us = us & s_mask;

		return 2.f * (float(UNorm10(us)) - 0.5f);

	//	return 2.f * (float(UNorm10(unsigned short(unsigned short(v >> 20) & s_mask))) - 0.5f);
	}

	float get_w() const
	{
		unsigned char uc(v >> 30);

		return 2.f * (float(UNorm2(uc)) - 0.5f);

	//	return 2.f * (float(UNorm2(unsigned char(v >> 30))) - 0.5f);
	}

	float3 get_vector3() const
	{
		return float3(get_x(), get_y(), get_z());
	}

	bool operator==(const Vector4_UNorm1010102 &a) const
	{
		return v == a.v;
	}
};
