#pragma once

#include "Math/Vector.hpp"
#include "Rendering/Color.hpp"
#include <string>
#include <cstdint>

class Cached_image;

struct Export_item
{
	enum class Selection
	{
		Red,
		Green,
		Blue,
		Alpha,
		RGB,
		Luminance,
		Constant,
		Ignore
	};

	Cached_image* cached_image;

	Selection selection;

	bool inverted;

	float modulate_factor;

	rendering::Color3 constant;
};

struct Export_set
{
	Export_item color;
	Export_item opacity;
	Export_item emissive;
	Export_item normals;
	Export_item height;
	Export_item metallic;
	Export_item roughness;
	Export_item cavity;
};
