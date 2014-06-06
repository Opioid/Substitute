#include "Texture_storage_save.hpp"
#include "Rendering/Texture_data_adapter.hpp"
#include <gli/gli.hpp>
#include <gli/core/save_dds.hpp>

namespace rendering
{

namespace texture_storage
{

bool save_DDS(const std::string& file_name, const Texture_data_adapter& adapter)
{
	if (file_name.empty())
	{
		return false;
	}

	const Texture_description& description = adapter.description();

	gli::format format;

	switch (description.format)
	{
	case Data_format::R8G8B8A8_UNorm:
		format = gli::RGBA8_UNORM;
		break;
	case Data_format::R16G16_UNorm:
		format = gli::RG16_UNORM;
		break;
	case Data_format::R16G16B16A16_Float:
		format = gli::RGBA16F;
		break;
	default:
		format = gli::FORMAT_NULL;
	}

	if (gli::FORMAT_NULL == format)
	{
		return false;
	}

	if (Texture_description::Type::Texture_2D == description.type)
	{
		gli::texture2D texture(1, format, gli::texture2D::dimensions_type(description.dimensions.x, description.dimensions.y));

		Texture_description::Data data;
		adapter.query_image(data, 0, 0, 0);

		std::copy(data.buffer, data.buffer + data.num_bytes, texture.data<unsigned char>());

		gli::save_dds(texture, file_name.c_str());
	}
	else if (Texture_description::Type::Texture_cube == description.type)
	{
		gli::textureCube texture(6, description.num_mip_levels, format, gli::textureCube::dimensions_type(description.dimensions.x, description.dimensions.y));

		for (uint32_t f = 0; f < description.num_faces; ++f)
		{
			gli::texture2D face = texture[f];

			for (uint32_t i = 0; i < description.num_mip_levels; ++i)
			{
				Texture_description::Data data;
				adapter.query_image(data, 0, f, i);

				std::copy(data.buffer, data.buffer + data.num_bytes, face[i].data<unsigned char>());
			}
		}

		gli::save_dds(texture, file_name.c_str());
	}

	return true;
}

}

}
