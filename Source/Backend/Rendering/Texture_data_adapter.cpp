#include "Texture_data_adapter.hpp"
#include <algorithm>

namespace rendering
{

Texture_data_adapter::Texture_data_adapter(const Texture_description& description) : description_(description)
{}

Texture_data_adapter::~Texture_data_adapter()
{}

const Texture_description& Texture_data_adapter::get_description() const
{
	return description_;
}

uint32_t Texture_data_adapter::get_num_images() const
{
	return description_.num_layers * description_.num_mip_levels;
}

Generic_texture_data_adapter::Generic_texture_data_adapter(const Texture_description& description) : Texture_data_adapter(description), owns_memory_(true)
{
	if (Texture_description::Type::Texture_cube == description_.type)
	{
		description_.num_layers = 6;
	}

	const uint32_t num_images = get_num_images();

	levels_ = new Texture_description::Data[num_images];

	for (uint32_t i = 0; i < description_.num_layers; ++i)
	{
		uint3 dimensions = description.dimensions;

		for (uint32_t j = 0; j < description_.num_mip_levels; ++j)
		{
			auto& level = levels_[i * description_.num_mip_levels + j];

			level.dimensions = dimensions;
			uint32_t bytes_per_pixel = static_cast<uint32_t>(Data_format::size_of(description.format));
			level.num_bytes = level.dimensions.x * level.dimensions.y * std::max(level.dimensions.z, uint32_t(1)) * bytes_per_pixel;
			level.buffer = new unsigned char[level.num_bytes];

			dimensions.x = std::max(dimensions.x / 2, uint32_t(1));
			dimensions.y = std::max(dimensions.y / 2, uint32_t(1));
			dimensions.z = std::max(dimensions.z / 2, uint32_t(1));
		}
	}
}

Generic_texture_data_adapter::Generic_texture_data_adapter(const Texture_description& description, Texture_description::Data* levels, bool owns_memory) :
	Texture_data_adapter(description), levels_(levels), owns_memory_(owns_memory)
{}

Generic_texture_data_adapter::~Generic_texture_data_adapter()
{
	if (owns_memory_)
	{
		const uint32_t num_images = get_num_images();
		for (uint32_t i = 0; i < num_images; ++i)
		{
			delete [] levels_[i].buffer;
		}

		delete [] levels_;
	}
}

bool Generic_texture_data_adapter::get_level(Texture_description::Data& data, uint32_t level) const
{
	data = levels_[level];

	return true;
}

bool Generic_texture_data_adapter::get_level(Texture_description::Data& data, uint32_t layer, uint32_t level) const
{
	data = levels_[layer * description_.num_mip_levels + level];

	return true;
}

}
