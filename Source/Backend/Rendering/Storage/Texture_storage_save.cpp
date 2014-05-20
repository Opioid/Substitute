#include "Texture_storage_save.hpp"
#include "Texture_storage_save_DDS.hpp"
#include "Texture_storage_save_SUI.hpp"
#include "Rendering/Texture_data_adapter.hpp"
#include <FreeImage.h>
#include <cstring>

namespace rendering
{

namespace texture_storage
{

bool save(const std::string& file_name, const Texture_data_adapter& adapter, file::File_type type)
{
	if (file::File_type::DDS == type)
	{
		return save_DDS(file_name, adapter);
	}
	else if (file::File_type::SUI == type)
	{
		return save_SUI(file_name, adapter);
	}

	const Texture_description& description = adapter.get_description();

	FIBITMAP* image = FreeImage_Allocate(description.dimensions.x, description.dimensions.y, 32);

	BYTE* bytes = FreeImage_GetBits(image);

	Texture_description::Data data;
	adapter.get_level(data, 0);

	memcpy(bytes, data.buffer, data.num_bytes);

	for (uint32_t y = 0; y < description.dimensions.y; ++y)
	{
		for (uint32_t x = 0; x < description.dimensions.x * 4; x += 4)
		{
			unsigned char* e0 = &bytes[y * description.dimensions.x * 4 + x + 0];
			unsigned char* e2 = &bytes[y * description.dimensions.x * 4 + x + 2];

			unsigned char r = *e0;
			unsigned char b = *e2;

			*e0 = b;
			*e2 = r;
		}
	}

	if (!FreeImage_Save(FIF_PNG, image, file_name.c_str(), 0))
	{
		FreeImage_Unload(image);
		return false;
	}

	FreeImage_Unload(image);

	return true;
}

}

}
