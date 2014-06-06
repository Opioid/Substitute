#include "Texture_storage_load_DDS.hpp"
#include "GLI_texture_data_adapter.hpp"

#include <iostream>

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


	gli::texture3D texture(storage);

/*
	if (1 < storage.layers())
	{
	//	std::cout << stream.name() << ": " << storage.la << std::endl;

		gli::texture2DArray texture(storage);

		auto dimensions = texture.dimensions();
		std::cout << stream.name() << ": " << texture. << std::endl;

	}
	*/
	auto dimensions = storage.dimensions(0);

	if (1 == dimensions.z)
	{

	}

	if (6 == storage.faces())
	{
		description.type = Texture_description::Type::Texture_cube;
		description.dimensions.x = dimensions.x;
		description.dimensions.y = dimensions.y;
		description.dimensions.z = dimensions.z;

	//	description.num_layers = static_cast<uint32_t>(storage.layers());
		description.num_faces = static_cast<uint32_t>(storage.faces());
		description.num_mip_levels = static_cast<uint32_t>(storage.levels());
	}
	else
	{
		description.type = Texture_description::Type::Texture_2D;
		description.dimensions.x = dimensions.x;
		description.dimensions.y = dimensions.y;
		description.dimensions.z = dimensions.z;

//		description.num_layers = static_cast<uint32_t>(storage.layers());
		description.num_faces = static_cast<uint32_t>(storage.faces());
		description.num_mip_levels = static_cast<uint32_t>(storage.levels());

	}

	return std::make_shared<GLI_texture_data_adapter>(description, storage);
}

}

}
