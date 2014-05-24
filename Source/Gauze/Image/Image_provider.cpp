#include "Image_provider.hpp"
#include "Image.hpp"
#include "Rendering/Data_format.hpp"
#include "Image_storage/Image_loader.hpp"
#include "Filter/To_float.hpp"

Cached_image::Cached_image(const Image<rendering::Color4c>* source_image) :
	source_image_(source_image),
	float_image_buffer_(nullptr),
	last_float_access_sRGB_(true)
{}

Cached_image::~Cached_image()
{
	delete float_image_buffer_;
	delete source_image_;
}

const Image<rendering::Color4c>& Cached_image::source_image() const
{
	return *source_image_;
}

const Image_buffer<rendering::Color4>& Cached_image::float_image_buffer(bool sRGB)
{
	if (!float_image_buffer_)
	{
		const Image_buffer<rendering::Color4c>* source = source_image_->get_level(0);

		float_image_buffer_ = new Image_buffer<rendering::Color4>(rendering::Data_format::R32G32B32A32_Float, source->dimensions());

		RGBA_to_float to_float(sRGB);
		to_float.filter(*float_image_buffer_, *source);
	}
	else if (sRGB != last_float_access_sRGB_)
	{
		const Image_buffer<rendering::Color4c>* source = source_image_->get_level(0);

		RGBA_to_float to_float(sRGB);
		to_float.filter(*float_image_buffer_, *source);
	}

	last_float_access_sRGB_ = sRGB;

	return *float_image_buffer_;
}

Image_provider::Image_provider()
{}

Image_provider::~Image_provider()
{
	for (auto i : cached_images_)
	{
		delete i.second;
	}
}

Cached_image* Image_provider::get_image(const std::string& file_name)
{
	auto i = cached_images_.find(file_name);

	if (cached_images_.end() != i)
	{
		return i->second;
	}

    Image<rendering::Color4c>* image = Image_loader::load(file_name);

	if (!image)
	{
		return nullptr;
	}

	Cached_image* cached_image = new Cached_image(image);

	cached_images_[file_name] = cached_image;

	return cached_image;
}
