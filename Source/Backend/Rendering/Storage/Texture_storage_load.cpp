#include "Texture_storage_load.hpp"
#include "Texture_storage_load_DDS.hpp"
#include "Texture_storage_load_SUI.hpp"
#include "Rendering/Texture_data_adapter.hpp"
#include "Texture_data_adapter_cube.hpp"
#include "File/File_type.hpp"
#include "FreeImage_helper.hpp"
#include "String/String.hpp"
#include "Parsing/Json.hpp"

namespace rendering
{

std::shared_ptr<Texture_data_adapter> Texture_storage::load_texture_2D(file::Input_stream& stream, bool treat_as_linear, std::string& error_message)
{
	file::File_type file_type = query_file_type(stream);

	if (file::File_type::DDS == file_type)
	{
		return texture_storage::load_DDS_texture_2D(stream, treat_as_linear, error_message);
	}

	FreeImageIO io;
	io.read_proc = fi::read; // pointer to function that calls fread
	io.write_proc = nullptr; // not needed for loading
	io.seek_proc = fi::seek; // pointer to function that calls fseek
	io.tell_proc = fi::tell; // pointer to function that calls ftell

	fi_handle fi_stream = reinterpret_cast<fi_handle>(&stream);

	FREE_IMAGE_FORMAT format = FreeImage_GetFileTypeFromHandle(&io, fi_stream);

	if (!FreeImage_FIFSupportsReading(format))
	{
		return nullptr;
	}

	stream.seekg(0, std::ios::beg);

	FIBITMAP* image = FreeImage_LoadFromHandle(format, &io, fi_stream);

	if (!image)
	{
		FreeImage_SetOutputMessage(fi::error_handler);
		error_message = fi::error_message();
		return nullptr;
	}

	Texture_description description;
	description.dimensions.x = FreeImage_GetWidth(image);
	description.dimensions.y = FreeImage_GetHeight(image);
	description.dimensions.z = 0;
	description.num_mip_levels = 1;

	uint32_t bpp = FreeImage_GetBPP(image);

	FIBITMAP* converted_image;

	if (bpp != 32)
	{
		converted_image = FreeImage_ConvertTo32Bits(image);
		FreeImage_Unload(image);
	}
	else
	{
		converted_image = image;
	}

	uint32_t new_pitch = FreeImage_GetPitch(converted_image);

	Texture_description::Data* data = new Texture_description::Data[1];
	data[0].dimensions = description.dimensions;
	data[0].buffer = new unsigned char[description.dimensions.x * description.dimensions.y * 4];

	FreeImage_ConvertToRawBits(data[0].buffer, converted_image, new_pitch, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, true);
	FreeImage_Unload(converted_image);

	for (uint32_t y = 0; y < description.dimensions.y; ++y)
	{
		for (uint32_t x = 0; x < description.dimensions.x * 4; x += 4)
		{
			unsigned char* e0 = &data[0].buffer[y * description.dimensions.x * 4 + x + 0];
			unsigned char* e2 = &data[0].buffer[y * description.dimensions.x * 4 + x + 2];

			unsigned char r = *e0;
			unsigned char b = *e2;

			*e0 = b;
			*e2 = r;
		}
	}

	if (treat_as_linear)
	{
		description.format = Data_format::Value::R8G8B8A8_UNorm;
	}
	else
	{
		description.format = Data_format::Value::R8G8B8A8_UNorm_sRGB;
	}

	return std::make_shared<Generic_texture_data_adapter>(description, data, true);
}

std::shared_ptr<Texture_data_adapter> Texture_storage::load_texture_3D(file::Input_stream& stream, bool /*treat_as_linear*/, std::string& error_message)
{
	return texture_storage::load_SUI_texture(stream, error_message);
}

std::shared_ptr<Texture_data_adapter> Texture_storage::load_texture_cube(file::Input_stream& stream, bool treat_as_linear, std::string& error_message)
{
	file::File_type file_type = query_file_type(stream);

	if (file::File_type::DDS == file_type)
	{
		return texture_storage::load_DDS_texture_cube(stream, treat_as_linear, error_message);
	}
	else if (file::File_type::SUI == file_type)
	{
		return texture_storage::load_SUI_texture(stream, error_message);
	}

	rapidjson::Document root;
	if (!json::parse(root, stream))
	{
		error_message = "Texture_storage::load_texture_cube(): \"" + stream.name() + "\" could not be loaded: failed to parse the file: " + json::read_error(root, stream);

		return nullptr;
	}

	std::shared_ptr<Texture_data_adapter> texture_data[6];

	for (auto n = root.MemberBegin(); n != root.MemberEnd(); ++n)
	{
		const std::string node_name = n->name.GetString();
		const rapidjson::Value& node_value = n->value;

		if (node_name == "type")
		{
		}
		else if (node_name == "encoding")
		{
		}
		else if (node_name == "sources")
		{
			if (!node_value.IsArray())
			{
				return nullptr;
			}

			for (rapidjson::SizeType i = 0; i < node_value.Size() && i < 6; ++i)
			{
				std::string name_2D = node_value[i].GetString();

				file::Input_stream stream_2D(name_2D);

				if (!stream_2D)
				{
					error_message = "Texture_storage::load_texture_cube(): \"" + name_2D + "\" could not be loaded.";

					return nullptr;
				}

				texture_data[i] = load_texture_2D(stream_2D, treat_as_linear, error_message);

				if (!texture_data[i])
				{
					return nullptr;
				}
			}
		}
	}

	Texture_description description = texture_data[0]->description();

	return std::make_shared<Texture_data_adapter_cube>(description, texture_data);
}

}
