#include "NVTT_output_handler.hpp"
#include <algorithm>

#include <iostream>

NVTT_output_handler::NVTT_output_handler(Image_buffer<rendering::Color4c>& image_buffer) :
	image_buffer_(image_buffer),
	target_bytes(reinterpret_cast<char*>(image_buffer.get_data())),
	receiving_image_data_(false),
	faulty_(false)
{}

void NVTT_output_handler::beginImage(int size, int width, int height, int /*depth*/, int /*face*/, int /*mip_level*/)
{
	receiving_image_data_ = true;

	dimensions_ = uint2(uint32_t(width), uint32_t(height));

	if (image_buffer_.get_num_bytes() < uint32_t(size))
	{
		faulty_ = true;
	}
}

bool NVTT_output_handler::writeData(const void* data, int size)
{
	if (!receiving_image_data_)
	{
		return true;
	}

	if (faulty_)
	{
		return false;
	}

	const char* source_bytes = static_cast<const char*>(data);

	std::copy(source_bytes, source_bytes + size, target_bytes);

	target_bytes += size;

    return true;
}

const uint2& NVTT_output_handler::get_dimensions() const
{
	return dimensions_;
}
