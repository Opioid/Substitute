#pragma once

#include "Image/Image_buffer.hpp"
#include <algorithm>

template<typename T>
class Copy
{

public:

	bool filter(Image_buffer<T>& destination, const Image_buffer<T>& source) const
	{
		if (destination.get_format()     != source.get_format()
		||  destination.get_dimensions() != source.get_dimensions())
		{
			return false;
		}

		const T* source_data = source.get_data();

		std::copy(source_data, source_data + source.get_num_pixels(), destination.get_data());

		return true;
	}
};
