#include "Texture_storage_load_DDS.hpp"
#include "GLI_texture_data_adapter.hpp"

namespace rendering
{

namespace texture_storage
{

std::shared_ptr<Texture_data_adapter> load_DDS_texture(file::Input_stream& stream, bool treat_as_linear, std::string& error_message)
{
	gli::storage storage(gli::load_dds(stream));

	if (storage.empty())
	{
		error_message = "File not found :(";
		return nullptr;
	}

	Texture_description description;
	description.format = GLI_texture_data_adapter::map(storage.format());

	if (!treat_as_linear)
	{
		description.format = Data_format::to_sRGB(description.format);
	}

	if (6 == storage.faces())
	{
		gli::textureCube texture(storage);

		description.type = Texture_description::Type::Texture_cube;
		description.dimensions.x = texture.dimensions().x;
		description.dimensions.y = texture.dimensions().y;
		description.dimensions.z = 1;
		description.num_mip_levels = static_cast<uint32_t>(texture.levels());
		description.num_layers = 6;

		return std::make_shared<GLI_texture_data_adapter>(description, texture);
	}
	else
	{
		gli::texture2D texture(storage);

		description.type = Texture_description::Type::Texture_2D;
		description.dimensions.x = texture.dimensions().x;
		description.dimensions.y = texture.dimensions().y;
		description.dimensions.z = 1;
		description.num_mip_levels = static_cast<uint32_t>(texture.levels());

		return std::make_shared<GLI_texture_data_adapter>(description, texture);
	}
}

}

}
