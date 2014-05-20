#include "Texture_data_adapter_cube.hpp"

namespace rendering
{

Texture_data_adapter_cube::Texture_data_adapter_cube(const Texture_description& description, const std::shared_ptr<Texture_data_adapter> texture_data[6])
	: Texture_data_adapter(description)
{
	description_.type = Texture_description::Type::Texture_cube;
	description_.num_layers = 6;

	texture__data[0] = texture_data[0];
	texture__data[1] = texture_data[1];
	texture__data[2] = texture_data[2];
	texture__data[3] = texture_data[3];
	texture__data[4] = texture_data[4];
	texture__data[5] = texture_data[5];
}

Texture_data_adapter_cube::~Texture_data_adapter_cube()
{}

bool Texture_data_adapter_cube::get_level(Texture_description::Data& data, uint32_t level) const
{
	return texture__data[0]->get_level(data, level);
}

bool Texture_data_adapter_cube::get_level(Texture_description::Data& data, uint32_t layer, uint32_t level) const
{
	return texture__data[layer]->get_level(data, level);
}

}
