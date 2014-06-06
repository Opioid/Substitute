#include "Texture_storage_load_SUI.hpp"
#include "Rendering/Texture_data_adapter.hpp"
#include "String/String.hpp"
#include <cstring>

namespace rendering
{

namespace texture_storage
{

std::shared_ptr<Texture_data_adapter> load_SUI_texture(file::Input_stream& stream, std::string& error_message)
{
	if (!stream)
	{
		error_message = "File not found :(";
		return nullptr;
	}

	char header[3];
	stream.read(header, sizeof(char) * 3);

	if (strncmp("SUI", header, 3))
	{
		return nullptr;
	}

	unsigned char version = 0;
	stream.read((char*)&version, sizeof(unsigned char));

	if (1 != version)
	{
		return nullptr;
	}

	Texture_description description;
	stream.read((char*)&description, sizeof(Texture_description));

	std::shared_ptr<Texture_data_adapter> adapter = std::make_shared<Generic_texture_data_adapter>(description);

	for (uint32_t l = 0; l < description.num_layers; ++l)
	{
		uint32_t num_faces = description.num_faces();
		for (uint32_t f = 0; f < num_faces; ++f)
		{
			for (uint32_t i = 0; i < description.num_mip_levels; ++i)
			{
				Texture_description::Data data;
				adapter->query_image(data, l, f, i);

				stream.read((char*)data.buffer, data.num_bytes);
			}
		}
	}

	return adapter;
}

}

}
