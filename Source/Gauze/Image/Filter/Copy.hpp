#pragma once

#include "Image/Image_buffer.hpp"
#include <algorithm>

template<typename T>
class Copy
{

public:

	bool filter(Image_buffer<T>& destination, const Image_buffer<T>& source) const
	{
		if (destination.format()     != source.format()
		||  destination.dimensions() != source.dimensions())
		{
			return false;
		}

		const T* source_data = source.data();

		std::copy(source_data, source_data + source.num_pixels(), destination.data());

		return true;
	}
};
