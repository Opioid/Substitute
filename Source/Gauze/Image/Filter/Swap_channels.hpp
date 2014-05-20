#pragma once

#include "Image/Image_buffer.hpp"
#include "Rendering/Color.hpp"
#include <cstdint>

class Swap_channels
{

public:

	Swap_channels(uint32_t channel_a, uint32_t channel_b) : channel_a_(channel_a), channel_b_(channel_b)
	{}

	bool filter(Image_buffer<rendering::Color4c>& destination, const Image_buffer<rendering::Color4c>& source) const
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
				const rendering::Color4c& source_color = source.get(uint2(x, y));

				rendering::Color4c destination_color = source_color;

				destination_color.v[channel_b_] = source_color.v[channel_a_];
				destination_color.v[channel_a_] = source_color.v[channel_b_];

				destination.set(destination_color, uint2(x, y));
			}
		}

		return true;
	}

private:

	uint32_t channel_a_;
	uint32_t channel_b_;
};
