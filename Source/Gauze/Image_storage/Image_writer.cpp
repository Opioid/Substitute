#include "Image_writer.hpp"
#include "Storage_options.hpp"
#include "Image/Image.hpp"
#include "Image/Colorspace.hpp"
#include "Image/Filter/To_BGRA.hpp"
#include "Math/Vector.inl"
#include <QImage>

Image_writer::Image_writer() : buffer_(rendering::Data_format::R8G8B8A8_Typeless, uint2(2048, 2048))
{}

bool Image_writer::write(const std::string& file_name, const Image<rendering::Color4>& image, const Storage_options& options)
{
	switch (options.get_container())
	{
	case Storage_options::Container::DDS:
		return write_DDS(file_name, image, options);
	case Storage_options::Container::PNG:
		return write_PNG(file_name, image, options);
	default:
		return false;
	}
}

bool Image_writer::write_DDS(const std::string& file_name, const Image<rendering::Color4>& image, const Storage_options& options)
{
	const Image_buffer<rendering::Color4>* source = image.get_level(0);
	const uint2& dimensions = source->dimensions();

	bool sRGB = rendering::Data_format::is_sRGB(options.format());

	To_BGRA to_BGRA(sRGB);

	allocate_buffer(dimensions);

	to_BGRA.filter_uncentered(buffer_, *source);

	nvtt::InputOptions input_options;

	if (!sRGB)
	{
		input_options.setGamma(1.f, 1.f);
	}

	input_options.setTextureLayout(nvtt::TextureType_2D, dimensions.x, dimensions.y);
	input_options.setMipmapData(buffer_.data(), dimensions.x, dimensions.y);
	input_options.setMipmapGeneration(true);

	nvtt::OutputOptions output_options;
	output_options.setFileName((file_name + "." + options.get_file_extension()).c_str());

	nvtt::Compressor compressor;

	nvtt::CompressionOptions compression_options;
	compression_options.setFormat(get_NVTT_format(options.format(), options.has_alpha()));
	compressor.process(input_options, compression_options, output_options);

	return false;
}

bool Image_writer::write_PNG(const std::string& file_name, const Image<rendering::Color4>& image, const Storage_options& options)
{
	bool sRGB = rendering::Data_format::is_sRGB(options.format());

	To_BGRA to_BGRA(sRGB);

	for (uint32_t i = 0; i < image.num_levels(); ++i)
	{
		const Image_buffer<rendering::Color4>* source = image.get_level(i);

		const uint2& dimensions = source->dimensions();

		allocate_buffer(dimensions);

		to_BGRA.filter_uncentered(buffer_, *source);

		QImage qimage(reinterpret_cast<const uchar*>(buffer_.data()), dimensions.x, dimensions.y, QImage::Format_ARGB32);

		qimage.save((file_name + std::to_string(i) + "." + options.get_file_extension()).c_str());
	}

	return true;
}

void Image_writer::allocate_buffer(const uint2& dimensions)
{
	const uint2& current_dimensions = buffer_.dimensions();

	if (current_dimensions.x < dimensions.x || current_dimensions.y < dimensions.y)
	{
		buffer_.resize(dimensions);
	}
}

nvtt::Format Image_writer::get_NVTT_format(rendering::Data_format::Value format, bool alpha)
{
	switch (format)
	{
	case rendering::Data_format::R8G8B8A8_SInt:
	case rendering::Data_format::R8G8B8A8_SNorm:
	case rendering::Data_format::R8G8B8A8_Typeless:
	case rendering::Data_format::R8G8B8A8_UInt:
	case rendering::Data_format::R8G8B8A8_UNorm:
	case rendering::Data_format::R8G8B8A8_UNorm_sRGB:
		return alpha ? nvtt::Format_RGBA : nvtt::Format_RGB;

	case rendering::Data_format::BC1_Typeless:
	case rendering::Data_format::BC1_UNorm:
	case rendering::Data_format::BC1_UNorm_sRGB:
		return alpha ? nvtt::Format_BC1a : nvtt::Format_BC1;

	case rendering::Data_format::BC2_Typeless:
	case rendering::Data_format::BC2_UNorm:
	case rendering::Data_format::BC2_UNorm_sRGB:
		return nvtt::Format_BC2;

	case rendering::Data_format::BC5_Typeless:
	case rendering::Data_format::BC5_UNorm:
	case rendering::Data_format::BC5_SNorm:
		return nvtt::Format_BC5;

	default:
		return alpha ? nvtt::Format_RGBA : nvtt::Format_RGB;
	}
}
