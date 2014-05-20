#pragma once

#include "Image/Image_buffer.hpp"
#include "Rendering/Color.hpp"
#include <cstdint>

class To_BGRA
{

public:

	To_BGRA(bool linear_to_sRGB) : linear_to_sRGB_(linear_to_sRGB)
	{}

	bool filter_uncentered(Image_buffer<rendering::Color4c>& destination, const Image_buffer<rendering::Color4>& source) const
	{
		uint32_t num_pixels = std::min(destination.get_num_pixels(), source.get_num_pixels());

		for (uint32_t i = 0; i < num_pixels; ++i)
		{
			const rendering::Color4& source_color = source.get(i);

			rendering::Color3 rgb = linear_to_sRGB_ ? rendering::linear_to_sRGB(source_color.rgb) : saturate(source_color.rgb);

			rendering::Color4c destination_color((unsigned char)(rgb.b * 255.f),
												 (unsigned char)(rgb.g * 255.f),
												 (unsigned char)(rgb.r * 255.f),
												 (unsigned char)(source_color.a * 255.f));

			destination.set(i, destination_color);
		}

		return true;
	}

	bool filter(Image_buffer<rendering::Color4c>& destination, const Image_buffer<rendering::Color4>& source) const
	{
		return filter(destination, source, source.get_dimensions());
	}

	bool filter(Image_buffer<rendering::Color4c>& destination, const Image_buffer<rendering::Color4>& source, const uint2& source_dimensions) const
	{
		const uint2& destination_dimensions = destination.get_dimensions();

		if (destination_dimensions.x < source_dimensions.x || destination_dimensions.y < source_dimensions.y)
		{
			return false;
		}

		const uint2 destination_offset = (destination_dimensions - source_dimensions) / 2;

//		const __m128 m4x255f = _mm_set_ps1(255.f);

		for (uint32_t y = 0; y < source_dimensions.y; ++y)
		{
			for (uint32_t x = 0; x < source_dimensions.x; ++x)
			{
				const rendering::Color4& source_color = source.get(uint2(x, y), source_dimensions.x);

				rendering::Color3 rgb = linear_to_sRGB_ ? rendering::linear_to_sRGB(source_color.rgb) : saturate(source_color.rgb);
/*
				__m128 m0 = _mm_set_ps(rgb.b, rgb.g, rgb.r, source_color.a);

				__m128 m1 = _mm_mul_ps(m0, m4x255f);

				__m128 m_min =_mm_min_ps(m1, m4x255f);

				__m128i mi = _mm_cvtps_epi32(m_min);

				rendering::Color4c destination_color(mi.m128i_i32[3], mi.m128i_i32[2], mi.m128i_i32[1], mi.m128i_i32[0]);
*/
				rendering::Color4c destination_color((unsigned char)(rgb.b * 255.f),
													 (unsigned char)(rgb.g * 255.f),
													 (unsigned char)(rgb.r * 255.f),
													 (unsigned char)(source_color.a * 255.f));

				destination.set(destination_offset + uint2(x, y), destination_color);
			}
		}

		return true;
	}

	bool filter(Image_buffer<rendering::Color4c>& destination, const Image_buffer<rendering::Color4c>& source) const
	{
		const uint2& source_dimensions = source.get_dimensions();
		const uint2& destination_dimensions = destination.get_dimensions();

		if (destination_dimensions.x < source_dimensions.x || destination_dimensions.y < source_dimensions.y)
		{
			return false;
		}

		if (destination.get_format() != source.get_format())
		{
			return false;
		}

		for (uint32_t y = 0; y < source.get_dimensions().y; ++y)
		{
			for (uint32_t x = 0; x < source.get_dimensions().x; ++x)
			{
				const rendering::Color4c& source_color = source.get(uint2(x, y));

				rendering::Color4c destination_color(source_color.b, source_color.g, source_color.r, source_color.a);

				destination.set(uint2(x, y), destination_color);
			}
		}

		return true;
	}

private:

	bool linear_to_sRGB_;
};
