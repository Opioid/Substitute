#include "Color.hpp"
#include "Math/Vector.hpp"

namespace rendering
{

namespace color3
{
	const Color3 white(1.f, 1.f, 1.f);
	const Color3 black(0.f, 0.f, 0.f);
	const Color3 red(1.f, 0.f, 0.f);
	const Color3 orange(1.f, 0.5f, 0.f);
	const Color3 yellow(1.f, 1.f, 0.f);
	const Color3 green(0.f, 1.f, 0.f);
	const Color3 blue(0.f, 0.f, 1.f);
}

namespace color4
{
	const Color4 white(1.f, 1.f, 1.f);
	const Color4 black(0.f, 0.f, 0.f);
	const Color4 red(1.f, 0.f, 0.f);
	const Color4 orange(1.f, 0.5f, 0.f);
	const Color4 yellow(1.f, 1.f, 0.f);
	const Color4 green(0.f, 1.f, 0.f);
	const Color4 blue(0.f, 0.f, 1.f);
}

// convert sRGB linear value to sRGB gamma value
float linear_to_sRGB(float c)
{
	if (c <= 0.f)
	{
		return 0.f;
	}
	else if (c < 0.0031308f)
	{
		return 12.92f * c;
	}
	else if (c < 1.f)
	{
		return 1.055f * std::pow(c, 0.41666f) - 0.055f;
	}
	else
	{
		return 1.f;
	}
}

// convert sRGB linear color to sRGB gamma color
Color3 linear_to_sRGB(const Color3& c)
{
	return Color3(linear_to_sRGB(c.r), linear_to_sRGB(c.g), linear_to_sRGB(c.b));
}

// convert sRGB gamma value to sRGB linear value
float sRGB_to_linear(float c)
{
	if (c <= 0.f)
	{
		return 0.f;
	}
	else if (c < 0.04045f)
	{
		return c / 12.92f;
	}
	else if (c < 1.f)
	{
		return std::pow((c + 0.055f) / 1.055f, 2.4f);
	}
	else
	{
		return 1.f;
	}
}

// convert sRGB gamma color to sRGB linear color
Color3 sRGB_to_linear(const Color3c& c)
{
	return Color3(sRGB_to_linear(float(c.r) / 255.f), sRGB_to_linear(float(c.g) / 255.f), sRGB_to_linear(float(c.b) / 255.f));
}

// convert sRGB gamma color to sRGB linear color
Color3 sRGB_to_linear(const Color3& c)
{
	return Color3(sRGB_to_linear(c.r), sRGB_to_linear(c.g), sRGB_to_linear(c.b));
}

// convert linear color to gamma color
Color3 linear_to_gamma(const Color3& c, float gamma)
{
	float p = 1.f / gamma;

	return Color3(std::pow(c.r, p), std::pow(c.g, p), std::pow(c.b, p));
}

// convert gamma color to linear color
Color3 gamma_to_linear(const Color3& c, float gamma)
{
	return Color3(std::pow(c.r, gamma), std::pow(c.g, gamma), std::pow(c.b, gamma));
}

Color3 to_float(const Color3c& c)
{
	return Color3(float(c.r) / 255.f, float(c.g) / 255.f, float(c.b) / 255.f);
}

}
