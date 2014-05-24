#pragma once

#include "Rendering/Color.hpp"
#include "Math/Vector.hpp"

template<typename T>
class Invert
{

public:

	bool filter(Image_buffer<T>& destination, const Image_buffer<T>& source) const
	{
		if (destination.format() != source.format()
		||  destination.dimensions() != source.dimensions())
		{
			return false;
		}

		T white;
		get_white(white);

		for (uint32_t y = 0; y < source.dimensions().y; ++y)
		{
			for (uint32_t x = 0; x < source.dimensions().x; ++x)
			{
				const T& source_color = source.get(uint2(x, y));

				T inverted = T(white.r - source_color.r, white.g - source_color.g, white.b - source_color.b, 255);

				destination.set(uint2(x, y), inverted);
			}
		}

		return true;
	}

private:

	static void get_white(rendering::Color4c& color)
	{
		color = rendering::Color4c(255, 255, 255, 255);
	}

	static void get_white(rendering::Color4& color)
	{
		color = rendering::Color4(1.f, 1.f, 1.f, 1.f);
	}
};

template<typename T>
class Invert_channel
{

public:

	Invert_channel(uint32_t channel) : channel_(channel)
	{}

	bool filter(Image_buffer<T>& destination, const Image_buffer<T>& source) const
	{
		if (destination.format() != source.format()
		||  destination.dimensions() != source.dimensions())
		{
			return false;
		}

		T white;
		get_white(white);

		for (uint32_t y = 0; y < source.dimensions().y; ++y)
		{
			for (uint32_t x = 0; x < source.dimensions().x; ++x)
			{
				const T& source_color = source.get(uint2(x, y));

				T inverted = source_color;

				inverted.v[channel_] = white.v[channel_] - source_color.v[channel_];

				destination.set(uint2(x, y), inverted);
			}
		}

		return true;
	}

private:

	static void get_white(rendering::Color4c& color)
	{
		color = rendering::Color4c(255, 255, 255, 255);
	}

	static void get_white(rendering::Color4& color)
	{
		color = rendering::Color4(1.f, 1.f, 1.f, 1.f);
	}

	uint32_t channel_;
};
