#pragma once

#include "Image_buffer.hpp"
#include "Rendering/Color.hpp"
#include <string>
#include <map>

template<typename T> class Image;

class Image_provider;

class Thumbnail_provider
{

public:

	Thumbnail_provider(Image_provider& image_provider, const uint2& thumbnail_dimensions);

	~Thumbnail_provider();

	const uint2& thumbnail_dimensions();

	const Image_buffer<rendering::Color4c>& checkerboard() const;

	Image_buffer<rendering::Color4>* get_thumbnail(const std::string& file_name);

	Image_buffer<rendering::Color4>& scratch_buffer();

private:

	Image_buffer<rendering::Color4>* create_thumbnail(const Image<rendering::Color4c>& image);

	void init_checkerboard();

	Image_provider& image_provider_;

	uint2 thumbnail_dimensions_;

	std::map<std::string, Image_buffer<rendering::Color4>*> thumbnails_;

    Image_buffer<rendering::Color4c> checkerboard_;

	Image_buffer<rendering::Color4> scratch_buffer_;

    Image_buffer<rendering::Color4c> scratch_buffer_8bit;
};
