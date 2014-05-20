#pragma once

#include "Rendering/Data_format.hpp"
#include "Math/Vector.hpp"
#include <algorithm>

template<typename T>
class Image_buffer
{

public:

	Image_buffer(rendering::Data_format::Value format = rendering::Data_format::Unknown, const uint2& dimensions = uint2(0, 0)) :
		format_(format),
		dimensions_(dimensions),
		num_bytes_(0),
		data_(nullptr)
	{
		resize(dimensions);
	}

	Image_buffer(rendering::Data_format::Value format, const uint2& dimensions, const T& color) :
		format_(format),
		dimensions_(dimensions),
		num_bytes_(0),
		data_(nullptr)
	{
		resize(dimensions);

		std::fill(data_, data_ + get_num_pixels(), color);
	}

	Image_buffer(const Image_buffer& source) :
		format_(source.format_),
		dimensions_(source.dimensions_),
		num_bytes_(source.num_bytes_)
	{
		if (0 == dimensions_.x || 0 == dimensions_.y)
		{
			return;
		}

		data_ = new T[dimensions_.x * dimensions_.y];

		std::copy(source.data_, source.data_ + source.get_num_pixels(), data_);
	}

	~Image_buffer()
	{
		delete [] data_;
	}

	void resize(const uint2& dimensions)
	{
		delete [] data_;

		dimensions_ = dimensions;

		if (0 == dimensions_.x || 0 == dimensions_.y)
		{
			num_bytes_ = 0;
			data_ = nullptr;
		}
		else
		{
			uint32_t byte_size = uint32_t(rendering::Data_format::size_of(format_));

			num_bytes_ = dimensions_.x * dimensions_.y * byte_size;
			data_      = new T[dimensions_.x * dimensions_.y];
		}
	}

	rendering::Data_format::Value get_format() const
	{
		return format_;
	}

	const uint2& get_dimensions() const
	{
		return dimensions_;
	}

	uint32_t get_num_pixels() const
	{
		return dimensions_.x * dimensions_.y;
	}

	uint32_t get_num_bytes() const
	{
		return num_bytes_;
	}

	const T* get_data() const
	{
		return data_;
	}

	T* get_data()
	{
		return data_;
	}

	const T& get(uint32_t pixel) const
	{
		return data_[pixel];
	}

	const T& get(const uint2 coord) const
	{
		return data_[coord.y * dimensions_.x + coord.x];
	}

	const T& get(const uint2 coord, uint32_t row_pitch) const
	{
		return data_[coord.y * row_pitch + coord.x];
	}

	void set(uint32_t pixel, const T& value)
	{
		data_[pixel] = value;
	}

	void set(const uint2& coord, const T& value)
	{
		data_[coord.y * dimensions_.x + coord.x] = value;
	}

	void clear_channel(uint32_t channel, float value)
	{
		const uint32_t num_pixels = get_num_pixels();

		for (uint32_t i = 0; i < num_pixels; ++i)
		{
			data_[i].v[channel] = value;
		}
	}

private:

	rendering::Data_format::Value format_;
	uint2                         dimensions_;

	uint32_t num_bytes_;
	T*       data_;
};
