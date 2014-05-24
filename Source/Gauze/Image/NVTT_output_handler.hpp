#pragma once

#include "Image_buffer.hpp"
#include "Rendering/Color.hpp"
#include <nvtt/nvtt.h>

class NVTT_output_handler : public nvtt::OutputHandler
{

public:

	NVTT_output_handler(Image_buffer<rendering::Color4c>& image_buffer);

    /// Indicate the start of a new compressed image that's part of the final texture.
	virtual void beginImage(int size, int width, int height, int depth, int face, int mip_level);

    /// Output data. Compressed data is output as soon as it's generated to minimize memory allocations.
	virtual bool writeData(const void* data, int size);

	const uint2& dimensions() const;

private:

	Image_buffer<rendering::Color4c> image_buffer_;

	uint2 dimensions_;

	char* target_bytes;

	bool receiving_image_data_;
	bool faulty_;
};
