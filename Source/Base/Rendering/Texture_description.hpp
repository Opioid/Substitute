#pragma once

#include "Data_format.hpp"
#include "Math/Vector.hpp"

namespace rendering
{

struct Texture_description
{
	enum class Type
	{
		Texture_1D,
		Texture_2D,
		Texture_3D,
		Texture_cube,
		Texture_2D_multisample
	};

	Texture_description();

	bool operator==(const Texture_description& other) const;

	Type     	       type;
	Data_format::Value format;
	uint3	           dimensions;
	uint32_t	       num_layers;
	uint32_t	       num_mip_levels;
	uint32_t		   num_samples;
	bool               shader_resource;

	struct Data
	{
		Data();

		uint3 	       dimensions;
		uint32_t       num_bytes;
		uint32_t       row_pitch;
		unsigned char* buffer;
	};

	static uint32_t calculate_num_mip_levels(uint3 dimensions);
};

}
