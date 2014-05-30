#include "Image_loader.hpp"
#include "Image/Image.hpp"
#include "Math/Vector.inl"
#include <QImage>

Image<rendering::Color4c>* Image_loader::load(const std::string& file_name)
{
	QImage source(file_name.c_str());

	if (source.isNull())
	{
		return nullptr;
	}

    Image<rendering::Color4c>* image = new Image<rendering::Color4c>(rendering::Data_format::B8G8R8A8_UNorm_sRGB, uint2(source.width(), source.height()));

    Image_buffer<rendering::Color4c>* destination = image->get_level(0);

	for (int y = 0; y < source.height(); ++y)
	{
		for (int x = 0; x < source.width(); ++x)
		{
			QRgb pixel = source.pixel(x, y);

            destination->set(uint2(x, y), rendering::Color4c(qRed(pixel), qGreen(pixel), qBlue(pixel), qAlpha(pixel)));

            /*
			rendering::Color3 rgb(float(qRed(pixel)) / 255.f, float(qGreen(pixel)) / 255.f, float(qBlue(pixel)) / 255.f);

			float alpha = float(qAlpha(pixel))  / 255.f;

			destination->set(uint2(x, y), rendering::Color4(rendering::sRGB_to_linear(rgb), alpha));
            */
		}
	}

	return image;
}
