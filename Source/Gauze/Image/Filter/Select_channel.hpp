#pragma once

#include "Image/Image_buffer.hpp"
#include "Rendering/Color.hpp"
#include <cstdint>

template<typename T>
class Select_channel
{

public:

	Select_channel(uint32_t destination_channel, uint32_t source_channel) : destination_channel_(destination_channel), source_channel_(source_channel)
	{}

	bool filter(Image_buffer<T>& destination, const Image_buffer<T>& source) const
	{
		if (destination.format() != source.format()
		||  destination.dimensions() != source.dimensions())
		{
			return false;
		}

		for (uint32_t y = 0; y < source.dimensions().y; ++y)
		{
			for (uint32_t x = 0; x < source.dimensions().x; ++x)
			{
				const T& source_color = source.get(uint2(x, y));

				T destination_color = destination.get(uint2(x, y));

				destination_color.v[destination_channel_] = source_color.v[source_channel_];

				destination.set(uint2(x, y), destination_color);
			}
		}

		return true;
	}

private:

	uint32_t destination_channel_;
	uint32_t source_channel_;
};
