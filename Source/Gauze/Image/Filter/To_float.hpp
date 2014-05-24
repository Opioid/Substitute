#pragma once

#include "Image/Image_buffer.hpp"
#include "Rendering/Color.hpp"
#include <cstdint>

class RGBA_to_float
{

public:

	RGBA_to_float(bool sRGB_to_linear) : sRGB_to_linear_(sRGB_to_linear)
	{}

    bool filter(Image_buffer<rendering::Color4>& destination, const Image_buffer<rendering::Color4c>& source) const
    {
		uint32_t num_pixels = std::min(destination.num_pixels(), source.num_pixels());

		for (uint32_t i = 0; i < num_pixels; ++i)
		{
			const rendering::Color4c& source_color = source.get(i);

			rendering::Color3 rgb = sRGB_to_linear_ ? rendering::sRGB_to_linear(source_color.rgb) : rendering::to_float(source_color.rgb);

			destination.set(i, rendering::Color4(rgb.r, rgb.g, rgb.b, float(source_color.a) / 255.f));
        }

        return true;
    }

private:

	bool sRGB_to_linear_;
};
