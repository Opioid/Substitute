#include "Thumbnail_provider.hpp"
#include "Image_provider.hpp"
#include "Image.hpp"
#include "Sampler/Point_sampler.hpp"
#include "Filter/To_float.hpp"
#include "Filter/Copy.hpp"
#include "Colorspace.hpp"
#include "NVTT_output_handler.hpp"

Thumbnail_provider::Thumbnail_provider(Image_provider& image_provider, const uint2& thumbnail_dimensions) :
	image_provider_(image_provider),
	thumbnail_dimensions_(thumbnail_dimensions),
    checkerboard_(rendering::Data_format::R8G8B8A8_UNorm_sRGB, thumbnail_dimensions),
	scratch_buffer_(rendering::Data_format::R32G32B32A32_Float, thumbnail_dimensions),
	scratch_buffer_8bit(rendering::Data_format::R8G8B8A8_UNorm_sRGB, thumbnail_dimensions)
{
	init_checkerboard();
}

Thumbnail_provider::~Thumbnail_provider()
{
	for (auto t : thumbnails_)
	{
		delete t.second;
	}
}

const uint2& Thumbnail_provider::get_thumbnail_dimensions()
{
	return thumbnail_dimensions_;
}

const Image_buffer<rendering::Color4c>& Thumbnail_provider::get_checkerboard() const
{
	return checkerboard_;
}

Image_buffer<rendering::Color4>* Thumbnail_provider::get_thumbnail(const std::string& file_name)
{
	auto t = thumbnails_.find(file_name);

	if (thumbnails_.end() != t)
	{
		return t->second;
	}

	Cached_image* cached_image = image_provider_.get_image(file_name);

	if (!cached_image)
	{
		return nullptr;
	}

	Image_buffer<rendering::Color4>* thumbnail = create_thumbnail(cached_image->get_source_image());

	thumbnails_[file_name] = thumbnail;

	return thumbnail;
}

Image_buffer<rendering::Color4>& Thumbnail_provider::get_scratch_buffer()
{
	return scratch_buffer_;
}

Image_buffer<rendering::Color4>* Thumbnail_provider::create_thumbnail(const Image<rendering::Color4c>& image)
{
	const Image_buffer<rendering::Color4c>* source = image.get_level(0);
	const uint2& dimensions = source->get_dimensions();

	if (dimensions.x <= thumbnail_dimensions_.x && dimensions.y <= thumbnail_dimensions_.y)
	{
		Image_buffer<rendering::Color4>* thumbnail = new Image_buffer<rendering::Color4>(rendering::Data_format::R32G32B32A32_Float, dimensions);

		RGBA_to_float to_float(true);
		to_float.filter(*thumbnail, *source);

		return thumbnail;
	}
	else
	{
		nvtt::InputOptions input_options;

		input_options.setTextureLayout(nvtt::TextureType_2D, dimensions.x, dimensions.y);
		input_options.setMipmapData(source->get_data(), dimensions.x, dimensions.y);
		input_options.setMaxExtents(thumbnail_dimensions_.x);
		input_options.setMipmapGeneration(false);

		nvtt::OutputOptions output_options;

		NVTT_output_handler output_handler(scratch_buffer_8bit);
		output_options.setOutputHandler(&output_handler);

		nvtt::Compressor compressor;

		nvtt::CompressionOptions compression_options;
		compression_options.setFormat(nvtt::Format_RGBA);
		compressor.process(input_options, compression_options, output_options);

		Image_buffer<rendering::Color4>* thumbnail = new Image_buffer<rendering::Color4>(rendering::Data_format::R32G32B32A32_Float, output_handler.get_dimensions());

		RGBA_to_float to_float(true);
		to_float.filter(*thumbnail, scratch_buffer_8bit);

		return thumbnail;
	}
}

void Thumbnail_provider::init_checkerboard()
{
    const rendering::Color4c light_color(153, 153, 153, 255);
    const rendering::Color4c dark_color (102, 102, 102, 255);

    rendering::Color4c* checkerboard_buffer = checkerboard_.get_data();

	bool toggle = true;

	size_t x_count = 0;
	size_t y_count = 0;

	for (size_t y = 0; y < thumbnail_dimensions_.y; ++y, ++y_count)
	{
		for (size_t x = 0; x < thumbnail_dimensions_.x; ++x, ++x_count)
		{
			if (x_count >= 8)
			{
				x_count = 0;
				toggle = !toggle;
			}

			checkerboard_buffer[y * thumbnail_dimensions_.x + x] = toggle ? light_color : dark_color;
		}

		if (y_count >= 8)
		{
			y_count = 0;
			toggle = !toggle;
		}
	}
}
