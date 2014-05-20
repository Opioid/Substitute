#include "Colorspace.hpp"

rendering::Color4c to_color4c_alpha_one(const rendering::Color4& color)
{
	return rendering::Color4c(static_cast<unsigned char>(color.r * 255.f),
							  static_cast<unsigned char>(color.g * 255.f),
							  static_cast<unsigned char>(color.b * 255.f),
							  0xFF);

//	return  0xFF000000 | (ir << 16) | (ig << 8) | ib;
}

rendering::Color4c to_color4c(const rendering::Color4& color)
{
	return rendering::Color4c(static_cast<unsigned char>(color.r * 255.f),
							  static_cast<unsigned char>(color.g * 255.f),
							  static_cast<unsigned char>(color.b * 255.f),
							  static_cast<unsigned char>(color.a * 255.f));
//	return  (ia << 24) | (ir << 16) | (ig << 8) | ib;
}

rendering::Color4c to_BGRA_order(const rendering::Color4c& color)
{
	return rendering::Color4c(color.b, color.g, color.r, color.a);
}
