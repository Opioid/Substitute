#include "Data_format_mapping.hpp"
#include <GL/glew.h>

namespace rendering
{

Data_format_mapping::Data_format_mapping(int internal_format, int format, int num_elements_, uint32_t type, bool normalized)
	: internal_format(internal_format), format(format), num_elements_(num_elements_), type(type), normalized(normalized ? GL_TRUE : GL_FALSE)
{}

Data_format_mapping Data_format_mapping::map(Data_format::Value format)
{
	switch (format)
	{
	case Data_format::R32G32B32A32_Float:
		return Data_format_mapping(GL_RGBA32F, GL_RGBA, 4, GL_FLOAT, false);
	case Data_format::R32G32B32_Float:
		return Data_format_mapping(GL_RGB32F, GL_RGB, 3, GL_FLOAT, false);
	case Data_format::R16G16B16A16_Float:
		return Data_format_mapping(GL_RGBA16F, GL_RGBA, 4, GL_HALF_FLOAT, false);
	case Data_format::R32G32_Float:
		return Data_format_mapping(GL_RG32F, GL_RG, 2, GL_FLOAT, false);
	case Data_format::R10G10B10A2_UNorm:
		return Data_format_mapping(GL_UNSIGNED_INT_10_10_10_2, GL_RGBA, 4, GL_UNSIGNED_INT_2_10_10_10_REV, true);
	case Data_format::R8G8B8A8_UNorm:
		return Data_format_mapping(GL_RGBA8, GL_RGBA, 4, GL_UNSIGNED_BYTE, true);
	case Data_format::R8G8B8A8_UNorm_sRGB:
		return Data_format_mapping(GL_SRGB8_ALPHA8, GL_RGBA, 4, GL_UNSIGNED_BYTE, true);
	case Data_format::R16G16_Float:
		return Data_format_mapping(GL_RG16F, GL_RG, 2, GL_HALF_FLOAT, false);
	case Data_format::R16G16_UNorm:
		return Data_format_mapping(GL_RG16, GL_RG, 2, GL_UNSIGNED_SHORT, true);
	case Data_format::D32_Float:
		return Data_format_mapping(GL_DEPTH_COMPONENT32F, GL_RED, 1, GL_FLOAT, false);
	case Data_format::R32_Float:
		return Data_format_mapping(GL_R32F, GL_RED, 1, GL_FLOAT, false);
	case Data_format::D24_UNorm_S8_UInt:
		return Data_format_mapping(GL_DEPTH24_STENCIL8_EXT, GL_RG, 2, GL_UNSIGNED_INT_24_8_EXT, false);
	case Data_format::R16_Float:
		return Data_format_mapping(GL_R16F, GL_RED, 1, GL_HALF_FLOAT, false);
	case Data_format::R16_UNorm:
		return Data_format_mapping(GL_R16, GL_RED, 1, GL_UNSIGNED_SHORT, true);
	case Data_format::R8_UNorm:
		return Data_format_mapping(GL_R8, GL_RED, 1, GL_UNSIGNED_BYTE, true);
	case Data_format::BC1_UNorm:
		return Data_format_mapping(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_RGBA, 4, GL_UNSIGNED_BYTE, true);
	case Data_format::BC1_UNorm_sRGB:
		return Data_format_mapping(GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, GL_RGBA, 4, GL_UNSIGNED_BYTE, true);
	case Data_format::BC2_UNorm:
		return Data_format_mapping(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_RGBA, 4, GL_UNSIGNED_BYTE, true);
	case Data_format::BC2_UNorm_sRGB:
		return Data_format_mapping(GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, GL_RGBA, 4, GL_UNSIGNED_BYTE, true);
	case Data_format::BC3_UNorm:
		return Data_format_mapping(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_RGBA, 4, GL_UNSIGNED_BYTE, true);
	case Data_format::BC3_UNorm_sRGB:
		return Data_format_mapping(GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, GL_RGBA, 4, GL_UNSIGNED_BYTE, true);
	case Data_format::BC4_UNorm:
		return Data_format_mapping(GL_COMPRESSED_RED_RGTC1, GL_R, 1, GL_UNSIGNED_BYTE, true);
	case Data_format::BC4_SNorm:
		return Data_format_mapping(GL_COMPRESSED_SIGNED_RED_RGTC1, GL_R, 1, GL_UNSIGNED_BYTE, true);
	case Data_format::BC5_UNorm:
		return Data_format_mapping(GL_COMPRESSED_RG_RGTC2, GL_RG, 2, GL_UNSIGNED_BYTE, true);
	case Data_format::BC5_SNorm:
		return Data_format_mapping(GL_COMPRESSED_SIGNED_RG_RGTC2, GL_RG, 2, GL_UNSIGNED_BYTE, true);
	case Data_format::BC6H_Uf16:
		return Data_format_mapping(GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT, GL_RG, 2, GL_UNSIGNED_BYTE, false);
	case Data_format::BC6H_Sf16:
		return Data_format_mapping(GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT, GL_RG, 2, GL_UNSIGNED_BYTE, false);
	case Data_format::BC7_UNorm:
		return Data_format_mapping(GL_COMPRESSED_RGBA_BPTC_UNORM, GL_RGBA, 4, GL_UNSIGNED_BYTE, true);
	case Data_format::BC7_UNorm_sRGB:
		return Data_format_mapping(GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM, GL_RGBA, 4, GL_UNSIGNED_BYTE, true);

	default:
		return Data_format_mapping();
	}

}

}
