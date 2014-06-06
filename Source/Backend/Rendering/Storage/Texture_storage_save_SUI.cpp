#include "Texture_storage_save.hpp"
#include "Rendering/Texture_data_adapter.hpp"
#include <fstream>

namespace rendering
{

namespace texture_storage
{

bool save_SUI(const std::string& file_name, const Texture_data_adapter& adapter)
{
	std::ofstream stream(file_name, std::ios::binary);

	if (!stream)
	{
		return false;
	}

	// Header
	const char header[] = "SUI\1";
	stream.write(header, sizeof(char) * 4);

	// Description
	const Texture_description& description = adapter.description();
	stream.write((char*)&description, sizeof(Texture_description));

	for (uint32_t l = 0; l < description.num_layers; ++l)
	{
		uint32_t num_faces = description.num_faces();
		for (uint32_t f = 0; f < num_faces; ++f)
		{
			for (uint32_t i = 0; i < description.num_mip_levels; ++i)
			{
				Texture_description::Data data;
				adapter.query_image(data, l, f, i);

				stream.write((char*)data.buffer, data.num_bytes);
			}
		}
	}

	return true;
}

}

}
