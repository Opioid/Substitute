#pragma once

#include "Rendering/Color.hpp"
#include "Math/Vector.hpp"

#include <iostream>

class Modulate
{

public:

	Modulate(float factor) : factor_(factor)
	{}

	bool filter(Image_buffer<rendering::Color4c>& destination, const Image_buffer<rendering::Color4c>& source) const
	{
		if (destination.get_format() != source.get_format()
		||  destination.get_dimensions() != source.get_dimensions())
		{
			return false;
		}

//		const __m128 m4x255f = _mm_set_ps1(255.f);

		for (uint32_t y = 0; y < source.get_dimensions().y; ++y)
		{
			for (uint32_t x = 0; x < source.get_dimensions().x; ++x)
			{
				const rendering::Color4c& source_color = source.get(uint2(x, y));
/*
				__m128i mi = _mm_set_epi32(source_color.b, source_color.g, source_color.r, 0);

				__m128 ma = _mm_cvtepi32_ps(mi);
				__m128 mb = _mm_set_ps1(factor_);

				__m128 m_mul = _mm_mul_ps(ma, mb);

				__m128 m_min =_mm_min_ps(m_mul, m4x255f);

				mi = _mm_cvtps_epi32(m_min);

				rendering::Color4c modulated(mi.m128i_i32[3], mi.m128i_i32[2], mi.m128i_i32[1], 255);
				*/

				rendering::Color4c modulated((unsigned char)(std::min(factor_ * float(source_color.r), 255.f)),
											 (unsigned char)(std::min(factor_ * float(source_color.g), 255.f)),
											 (unsigned char)(std::min(factor_ * float(source_color.b), 255.f)),
											 255);

				destination.set(uint2(x, y), modulated);
			}
		}

		return true;
	}

	bool filter(Image_buffer<rendering::Color4>& destination, const Image_buffer<rendering::Color4>& source) const
	{
		if (destination.get_format() != source.get_format()
		||  destination.get_dimensions() != source.get_dimensions())
		{
			return false;
		}

		for (uint32_t y = 0; y < source.get_dimensions().y; ++y)
		{
			for (uint32_t x = 0; x < source.get_dimensions().x; ++x)
			{
				const rendering::Color4& source_color = source.get(uint2(x, y));

				rendering::Color4 modulated(factor_ * source_color.r, factor_ * source_color.g, factor_ * source_color.b, 1.f);

				destination.set(uint2(x, y), modulated);
			}
		}

		return true;
	}

private:

	float factor_;
};

class Modulate_channel
{

public:

	Modulate_channel(uint32_t channel, float factor) : channel_(channel), factor_(factor)
	{}

	bool filter(Image_buffer<rendering::Color4>& destination, const Image_buffer<rendering::Color4>& source) const
	{
		if (destination.get_format() != source.get_format()
		||  destination.get_dimensions() != source.get_dimensions())
		{
			return false;
		}

		for (uint32_t y = 0; y < source.get_dimensions().y; ++y)
		{
			for (uint32_t x = 0; x < source.get_dimensions().x; ++x)
			{
				const rendering::Color4& source_color = source.get(uint2(x, y));

				rendering::Color4 modulated = source_color;

				modulated.v[channel_] = factor_ * source_color.v[channel_];

				destination.set(uint2(x, y), modulated);
			}
		}

		return true;
	}

private:

	uint32_t channel_;
	float factor_;
};
