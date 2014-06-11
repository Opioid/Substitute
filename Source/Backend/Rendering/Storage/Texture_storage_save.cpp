#include "Texture_storage_save.hpp"
#include "Texture_storage_save_DDS.hpp"
#include "Texture_storage_save_SUI.hpp"
#include "Texture_storage_save_WEBP.hpp"
#include "Rendering/Texture_data_adapter.hpp"
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
	else if (file::File_type::WEBP == type)
	{
		return save_WEBP(file_name, adapter);
	}

	return false;
}

}

}
