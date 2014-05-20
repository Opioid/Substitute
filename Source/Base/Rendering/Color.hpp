#pragma once

template<typename T> struct tVector3;
template<typename T> struct tVector4;

namespace rendering
{

typedef tVector3<unsigned char> Color3c;
typedef tVector3<float> Color3;
typedef tVector4<unsigned char> Color4c;
typedef tVector4<float> Color4;

namespace color3
{
	extern const Color3 white;
	extern const Color3 black;
	extern const Color3 red;
	extern const Color3 orange;
	extern const Color3 yellow;
	extern const Color3 green;
	extern const Color3 blue;
}

namespace color4
{
	extern const Color4 white;
	extern const Color4 black;
	extern const Color4 red;
	extern const Color4 orange;
	extern const Color4 yellow;
	extern const Color4 green;
	extern const Color4 blue;
}

// convert sRGB linear color to sRGB gamma color
Color3 linear_to_sRGB(const Color3& c);

// convert sRGB gamma color to sRGB linear color
Color3 sRGB_to_linear(const Color3c& c);
Color3 sRGB_to_linear(const Color3& c);

// convert linear color to gamma color
Color3 linear_to_gamma(const Color3& c, float gamma);

// convert gamma color to linear color
Color3 gamma_to_linear(const Color3& c, float gamma);

Color3 to_float(const Color3c& c);

}
