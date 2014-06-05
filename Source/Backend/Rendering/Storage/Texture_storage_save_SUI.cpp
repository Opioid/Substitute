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

	const uint32_t count = adapter.num_blocks();
	for (uint32_t i = 0; i < count; ++i)
	{
		Texture_description::Data data;
		adapter.get_level(data, i);

		stream.write((char*)data.buffer, data.num_bytes);
	}

	return true;
}

}

}
