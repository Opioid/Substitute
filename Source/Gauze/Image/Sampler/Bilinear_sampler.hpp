#pragma once

#include "Math/Vector.hpp"
#include "Image/Image_buffer.hpp"

template<typename T>
class Bilinear_sampler
{

public:

	Bilinear_sampler(const Image_buffer<T>& image_buffer) :
		data_(image_buffer.data()),
		dimensions_int_(image_buffer.dimensions()),
		dimensions_float_(image_buffer.dimensions())
	{}

	T sample(const float2& coord) const
	{
		uint2 offset0 = uint2(uint32_t(coord.x * dimensions_float_.x), uint32_t(coord.y * dimensions_float_.y));

		T sample0 = data_[offset.y * dimensions_int_.x + offset.x];

		return sample0;
	}

private:

	const T* data_;

	uint2  dimensions_int_;
	float2 dimensions_float_;
};
