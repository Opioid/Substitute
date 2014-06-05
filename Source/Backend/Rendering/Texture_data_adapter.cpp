#include "Texture_data_adapter.hpp"
#include <algorithm>

namespace rendering
{

Texture_data_adapter::Texture_data_adapter(const Texture_description& description) : description_(description)
{}

Texture_data_adapter::~Texture_data_adapter()
{}

const Texture_description& Texture_data_adapter::description() const
{
	return description_;
}

uint32_t Texture_data_adapter::num_blocks() const
{
	return description_.num_mip_levels * description_.num_layers;
}

Generic_texture_data_adapter::Generic_texture_data_adapter(const Texture_description& description) : Texture_data_adapter(description), owns_memory_(true)
{
	blocks_ = new Texture_description::Data[num_blocks()];

	for (uint32_t i = 0; i < description_.num_layers; ++i)
	{
		uint3 dimensions = description.dimensions;

		for (uint32_t j = 0; j < description_.num_mip_levels; ++j)
		{
			auto& block = blocks_[i * description_.num_mip_levels + j];

			block.dimensions = dimensions;
			uint32_t bytes_per_pixel = Data_format::num_bytes_per_block(description.format);
			block.num_bytes = block.dimensions.x * block.dimensions.y * std::max(block.dimensions.z, uint32_t(1)) * bytes_per_pixel;
			block.buffer = new unsigned char[block.num_bytes];

			dimensions.x = std::max(dimensions.x / 2, uint32_t(1));
			dimensions.y = std::max(dimensions.y / 2, uint32_t(1));
			dimensions.z = std::max(dimensions.z / 2, uint32_t(1));
		}
	}
}

Generic_texture_data_adapter::Generic_texture_data_adapter(const Texture_description& description, Texture_description::Data* blocks, bool owns_memory) :
	Texture_data_adapter(description), blocks_(blocks), owns_memory_(owns_memory)
{}

Generic_texture_data_adapter::~Generic_texture_data_adapter()
{
	if (owns_memory_)
	{
		const uint32_t count = num_blocks();
		for (uint32_t i = 0; i < count; ++i)
		{
			delete [] blocks_[i].buffer;
		}

		delete [] blocks_;
	}
}

bool Generic_texture_data_adapter::get_level(Texture_description::Data& data, uint32_t level) const
{
	data = blocks_[level];

	return true;
}

bool Generic_texture_data_adapter::get_image(Texture_description::Data& data, uint32_t level, uint32_t layer) const
{
	data = blocks_[layer * description_.num_mip_levels + level];

	return true;
}

}
