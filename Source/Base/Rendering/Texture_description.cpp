#include "Texture_description.hpp"
#include "Math/Math.hpp"
#include "Math/Vector.inl"

namespace rendering
{

Texture_description::Texture_description() :
	type(Type::Texture_2D), format(Data_format::Unknown),
	dimensions(uint3(1, 1, 1)), num_layers(1), num_mip_levels(1), num_samples(0),
	shader_resource(true)
{}

bool Texture_description::operator==(const Texture_description& other) const
{
	return type 	       == other.type
		&& format 	       == other.format
		&& dimensions      == other.dimensions
		&& num_layers      == other.num_layers
		&& num_mip_levels  == other.num_mip_levels
		&& num_samples     == other.num_samples
		&& shader_resource == other.shader_resource;
}

Texture_description::Data::Data() : row_pitch(0)
{}

uint32_t Texture_description::calculate_num_mip_levels(uint3 dimensions)
{
	uint32_t num_mip_levels = 1;

	while ((dimensions.x > 1) || (dimensions.y > 1) || (dimensions.z > 1))
	{
		dimensions.x = std::max(dimensions.x / 2, uint32_t(1));
		dimensions.y = std::max(dimensions.y / 2, uint32_t(1));
		dimensions.z = std::max(dimensions.z / 2, uint32_t(1));

		++num_mip_levels;
	}

	return num_mip_levels;
}

}
