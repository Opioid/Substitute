#pragma once

#include "Image/Image_buffer.hpp"
#include "Rendering/Color.hpp"
#include <nvtt/nvtt.h>
#include <string>

class Storage_options;

template<typename T> class Image;

class Image_writer
{

public:

	Image_writer();

	bool write(const std::string& file_name, const Image<rendering::Color4>& image, const Storage_options& options);

private:

	bool write_DDS(const std::string& file_name, const Image<rendering::Color4>& image, const Storage_options& options);

	bool write_PNG(const std::string& file_name, const Image<rendering::Color4>& image, const Storage_options& options);

	void allocate_buffer(const uint2& dimensions);

	static nvtt::Format get_NVTT_format(rendering::Data_format::Value, bool alpha);

	Image_buffer<rendering::Color4c> buffer_;
};
