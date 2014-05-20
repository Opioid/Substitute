#pragma once

#include "Image/Image_buffer.hpp"
#include "Rendering/Color.hpp"
#include <cstdint>

template<typename T>
class Channel_to_grayscale
{

public:

	Channel_to_grayscale(uint32_t channel) : channel_(channel)
	{}

	bool filter(Image_buffer<T>& destination, const Image_buffer<T>& source) const
	{
		uint32_t num_pixels = std::min(destination.get_num_pixels(), source.get_num_pixels());

		for (uint32_t i = 0; i < num_pixels; ++i)
		{
			const T& source_color = source.get(i);

			destination.set(i, T(source_color.v[channel_], source_color.v[channel_], source_color.v[channel_], source_color.v[channel_]));
		}

		return true;
	}

private:

	uint32_t channel_;
};
