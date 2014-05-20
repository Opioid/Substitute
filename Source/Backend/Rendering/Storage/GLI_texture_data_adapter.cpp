#include "GLI_texture_data_adapter.hpp"

namespace rendering
{

Data_format::Value GLI_texture_data_adapter::map(gli::format gli_format)
{
	switch (gli_format)
	{
	case gli::RGBA8_UNORM:
		return Data_format::R8G8B8A8_UNorm;
	case gli::RGB_DXT1:
	case gli::RGBA_DXT1:
		return Data_format::BC1_UNorm;
	case gli::RGBA_DXT3:
		return Data_format::BC2_UNorm;
	case gli::RGBA_DXT5:
		return Data_format::BC3_UNorm;
	case gli::R_ATI1N_UNORM:
		return Data_format::BC4_UNorm;
	case gli::RG_ATI2N_UNORM:
		return Data_format::BC5_UNorm;
	case gli::RG_ATI2N_SNORM:
		return Data_format::BC5_SNorm;
	case gli::RGB_BP_UNSIGNED_FLOAT:
		return Data_format::BC6H_Uf16;
	case gli::RGB_BP_SIGNED_FLOAT:
		return Data_format::BC6H_Sf16;
	case gli::RGB_BP_UNORM:
		return Data_format::BC7_UNorm;
	default:
		return Data_format::Unknown;
	}

}

GLI_texture_data_adapter_2D::GLI_texture_data_adapter_2D(const Texture_description& description, const gli::texture2D& texture) :
	Texture_data_adapter(description), texture_(texture)
{}

GLI_texture_data_adapter_2D::~GLI_texture_data_adapter_2D()
{}

bool GLI_texture_data_adapter_2D::get_level(Texture_description::Data& data, uint32_t level) const
{
	gli::image image = texture_[level];

	data.dimensions.x = image.dimensions().x;
	data.dimensions.y = image.dimensions().y;
	data.num_bytes = static_cast<uint32_t>(image.size());
	data.buffer = static_cast<unsigned char*>(image.data());

	return true;
}

bool GLI_texture_data_adapter_2D::get_level(Texture_description::Data& data, uint32_t layer, uint32_t level) const
{
	gli::image image = texture_[level];

	data.dimensions.x = image.dimensions().x;
	data.dimensions.y = image.dimensions().y;
	data.dimensions.z = 0;
	data.num_bytes = static_cast<uint32_t>(image.size());
	data.buffer = static_cast<unsigned char*>(image.data());

	return layer == 0;
}

GLI_texture_data_adapter_cube::GLI_texture_data_adapter_cube(const Texture_description& description, const gli::textureCube& texture) :
	Texture_data_adapter(description), texture_(texture)
{
	description_.type = Texture_description::Type::Texture_cube;
	description_.num_layers = 6;
}

GLI_texture_data_adapter_cube::~GLI_texture_data_adapter_cube()
{}

bool GLI_texture_data_adapter_cube::get_level(Texture_description::Data& data, uint32_t level) const
{
	gli::texture2D face = texture_[0];
	gli::image image = face[level];

	data.dimensions.x = image.dimensions().x;
	data.dimensions.y = image.dimensions().y;
	data.dimensions.z = 0;
	data.num_bytes = static_cast<uint32_t>(image.size());
	data.buffer = static_cast<unsigned char*>(image.data());

	return true;
}

bool GLI_texture_data_adapter_cube::get_level(Texture_description::Data& data, uint32_t layer, uint32_t level) const
{
	gli::texture2D face = texture_[layer];
	gli::image image = face[level];

	data.dimensions.x = image.dimensions().x;
	data.dimensions.y = image.dimensions().y;
	data.dimensions.z = 0;
	data.num_bytes = static_cast<uint32_t>(image.size());
	data.buffer = static_cast<unsigned char*>(image.data());

	return true;
}

}
