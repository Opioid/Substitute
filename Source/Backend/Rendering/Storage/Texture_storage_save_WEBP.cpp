#include "Texture_storage_save_WEBP.hpp"
#include "Rendering/Texture_data_adapter.hpp"
#include "webp/encode.h"
#include <fstream>

namespace rendering
{

namespace texture_storage
{

bool save_WEBP(const std::string& file_name, const Texture_data_adapter& adapter)
{
	const Texture_description& description = adapter.description();

	if (Data_format::R8G8B8A8_UNorm_sRGB != description.format)
	{
		return false;
	}

	std::ofstream stream(file_name, std::ios::binary);

	if (!stream)
	{
		return false;
	}

	Texture_description::Data data;
	adapter.query_image(data, 0, 0, 0);

	uint8_t* output;

	size_t num_bytes = WebPEncodeLosslessRGBA(reinterpret_cast<uint8_t*>(data.buffer), data.dimensions.x, data.dimensions.y, data.row_pitch, &output);

	if (!num_bytes)
	{
		return false;
	}

	stream.write(reinterpret_cast<char*>(output), num_bytes);

	free(output);

	return true;
}

}

}
