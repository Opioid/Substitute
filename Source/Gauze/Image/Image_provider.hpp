#pragma once

#include "Rendering/Color.hpp"
#include <string>
#include <map>

template<typename T> class Image_buffer;
template<typename T> class Image;

class Cached_image
{

public:

	Cached_image(const Image<rendering::Color4c>* source_image);

	~Cached_image();

	const Image<rendering::Color4c>& source_image() const;

	const Image_buffer<rendering::Color4>& float_image_buffer(bool sRGB = false);

private:

	const Image<rendering::Color4c>* source_image_;

	Image_buffer<rendering::Color4>* float_image_buffer_;

	bool last_float_access_sRGB_;
};

class Image_provider
{

public:

	Image_provider();

	~Image_provider();

	Cached_image* get_image(const std::string& file_name);

private:

	std::map<std::string, Cached_image*> cached_images_;

};
