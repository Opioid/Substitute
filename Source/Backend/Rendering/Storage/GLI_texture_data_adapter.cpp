#include "GLI_texture_data_adapter.hpp"

namespace rendering
{

GLI_texture_data_adapter::GLI_texture_data_adapter(const Texture_description& description, const gli::storage& storage) :
	Texture_data_adapter(description), storage_(storage)
{}

GLI_texture_data_adapter::~GLI_texture_data_adapter()
{}

bool GLI_texture_data_adapter::query_image(Texture_description::Data& data, uint32_t layer, uint32_t face, uint32_t level) const
{
	gli::image image;

	if (Texture_description::Type::Texture_2D == description_.type)
	{
/*		if (description_.num_layers > 1)
		{
			gli::texture2DArray texture_2D_array(storage_);
			gli::texture2D texture_2D(texture_2D_array[layer]);
			image = texture_2D[level];
		}
		else*/
		{
			gli::texture2D texture_2D(storage_);
			image = texture_2D[level];
		}
	}
	else if (Texture_description::Type::Texture_cube == description_.type)
	{
		gli::textureCube texture_cube(storage_);
		gli::texture2D texture_2D(texture_cube[face]);
		image = texture_2D[level];
	}

	data.dimensions.x = image.dimensions().x;
	data.dimensions.y = image.dimensions().y;
	data.dimensions.z = image.dimensions().z;
	data.num_bytes = static_cast<uint32_t>(image.size());
	data.buffer = static_cast<unsigned char*>(image.data());

	if (description_.num_layers > 1)
	{
		return false;
	}

	return true;
}

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

}
