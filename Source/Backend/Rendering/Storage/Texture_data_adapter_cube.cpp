#include "Texture_data_adapter_cube.hpp"

namespace rendering
{

Texture_data_adapter_cube::Texture_data_adapter_cube(const Texture_description& description, const std::shared_ptr<Texture_data_adapter> texture_data[6]) :
	Texture_data_adapter(description)
{
	description_.type = Texture_description::Type::Texture_cube;
	description_.num_layers = 6;

	texture_data_[0] = texture_data[0];
	texture_data_[1] = texture_data[1];
	texture_data_[2] = texture_data[2];
	texture_data_[3] = texture_data[3];
	texture_data_[4] = texture_data[4];
	texture_data_[5] = texture_data[5];
}

Texture_data_adapter_cube::~Texture_data_adapter_cube()
{}

bool Texture_data_adapter_cube::get_level(Texture_description::Data& data, uint32_t level) const
{
	return texture_data_[0]->get_level(data, level);
}

bool Texture_data_adapter_cube::get_image(Texture_description::Data& data, uint32_t level, uint32_t layer) const
{
	return texture_data_[layer]->get_level(data, level);
}

}
