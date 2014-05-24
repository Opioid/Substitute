#include "Data_format.hpp"

namespace rendering
{

bool Data_format::is_sRGB(Value format)
{
	switch (format)
	{
	case R8G8B8A8_UNorm_sRGB:
	case BC1_UNorm_sRGB:
	case BC2_UNorm_sRGB:
	case BC3_UNorm_sRGB:
	case BC7_UNorm_sRGB:
		return true;

	default:
		return false;
	}
}

Data_format::Value Data_format::to_sRGB(Value format)
{
	switch (format)
	{
	case R8G8B8A8_Typeless:
	case R8G8B8A8_UNorm:
		return R8G8B8A8_UNorm_sRGB;

	case BC1_Typeless:
	case BC1_UNorm:
		return BC1_UNorm_sRGB;

	case BC2_Typeless:
	case BC2_UNorm:
		return BC2_UNorm_sRGB;

	case BC3_Typeless:
	case BC3_UNorm:
		return BC3_UNorm_sRGB;

	case BC7_Typeless:
	case BC7_UNorm_sRGB:
		return BC7_UNorm_sRGB;

	default:
		return format;
	}
}

uint32_t Data_format::num_elements_per_block(Value format)
{
	switch (format)
	{
	case BC1_Typeless:
	case BC1_UNorm:
	case BC1_UNorm_sRGB:
	case BC2_Typeless:
	case BC2_UNorm:
	case BC2_UNorm_sRGB:
	case BC3_Typeless:
	case BC3_UNorm:
	case BC3_UNorm_sRGB:
	case BC4_Typeless:
	case BC4_UNorm:
	case BC4_SNorm:
	case BC5_Typeless:
	case BC5_UNorm:
	case BC5_SNorm:
	case BC6H_Typeless:
	case BC6H_Uf16:
	case BC6H_Sf16:
	case BC7_Typeless:
	case BC7_UNorm:
	case BC7_UNorm_sRGB:
		return 16;
	default:
		return 1;
	}
}

uint32_t Data_format::num_bytes_per_block(Value format)
{
	switch (format)
	{
	case R32G32B32A32_Typeless:
	case R32G32B32A32_Float:
	case R32G32B32A32_UInt:
	case R32G32B32A32_SInt:
		return 16;
	case R32G32B32_Typeless:
	case R32G32B32_Float:
	case R32G32B32_UInt:
	case R32G32B32_SInt:
		return 12;
	case R16G16B16A16_Typeless:
	case R16G16B16A16_Float:
	case R16G16B16A16_UNorm:
	case R16G16B16A16_UInt:
	case R16G16B16A16_SNorm:
	case R16G16B16A16_SInt:
	case R32G32_Typeless:
	case R32G32_Float:
	case R32G32_UInt:
	case R32G32_SInt:
		return 8;
	case R32G8X24_Typeless:
	case D32_Float_S8X24_UInt:
	case R32_Float_X8X24_Typeless:
	case X32_Typeless_G8X24_UInt:
	case R10G10B10A2_Typeless:
	case R10G10B10A2_UNorm:
	case R10G10B10A2_UInt:
	case R11G11B10_Float:
	case R8G8B8A8_Typeless:
	case R8G8B8A8_UNorm:
	case R8G8B8A8_UNorm_sRGB:
	case R8G8B8A8_UInt:
	case R8G8B8A8_SNorm:
	case R8G8B8A8_SInt:
	case R16G16_Typeless:
	case R16G16_Float:
	case R16G16_UNorm:
	case R16G16_UInt :
	case R16G16_SNorm:
	case R16G16_SInt:
	case R32_Typeless:
	case D32_Float:
	case R32_Float:
	case R32_UInt:
	case R32_SInt:
	case R24G8_Typeless:
	case D24_UNorm_S8_UInt:
	case R24_UNorm_X8_Typeless:
	case X24_Typeless_G8_UInt:
		return 4;
	case R8G8_Typeless:
	case R8G8_UNorm:
	case R8G8_UInt:
	case R8G8_SNorm:
	case R8G8_SInt:
	case R16_Typeless:
	case R16_Float:
	case D16_UNorm:
	case R16_UNorm:
	case R16_UInt:
	case R16_SNorm:
	case R16_SInt:
		return 2;
	case R8_Typeless:
	case R8_UNorm:
	case R8_UInt:
	case R8_SNorm:
	case R8_SInt:
		return 1;
	case R1_UNorm:
		return 1; //?!?
	case R9G9B9E5_SharedExp:
	case R8G8_B8G8_UNorm:
	case G8R8_G8B8_UNorm:
		return 4;
	case BC1_Typeless:
	case BC1_UNorm:
	case BC1_UNorm_sRGB:
		return 8;
	case BC2_Typeless:
	case BC2_UNorm:
	case BC2_UNorm_sRGB:
	case BC3_Typeless:
	case BC3_UNorm:
	case BC3_UNorm_sRGB:
		return 16;
	case BC4_Typeless:
	case BC4_UNorm:
	case BC4_SNorm:
		return 8;
	case BC5_Typeless:
	case BC5_UNorm:
	case BC5_SNorm:
		return 16;
	case B5G6R5_UNorm:
	case B5G5R5A1_UNorm:
		return 2;
	case B8G8R8A8_UNorm:
	case B8G8R8X8_UNorm:
	case R10G10B10_Xr_Bias_A2_UNorm:
	case B8G8R8A8_Typeless:
	case B8G8R8A8_UNorm_sRGB:
	case B8G8R8X8_Typeless:
	case B8G8R8X8_UNorm_sRGB:
		return 4;
	case BC6H_Typeless:
	case BC6H_Uf16:
	case BC6H_Sf16:
	case BC7_Typeless:
	case BC7_UNorm:
	case BC7_UNorm_sRGB:
		return 0;  //Block compressed?!
	default:
		return 0;
	}
}

bool Data_format::is_compressed(Value format)
{
	switch (format)
	{
	case BC1_Typeless:
	case BC1_UNorm:
	case BC1_UNorm_sRGB:
	case BC2_Typeless:
	case BC2_UNorm:
	case BC2_UNorm_sRGB:
	case BC3_Typeless:
	case BC3_UNorm:
	case BC3_UNorm_sRGB:
	case BC4_Typeless:
	case BC4_UNorm:
	case BC4_SNorm:
	case BC5_Typeless:
	case BC5_UNorm:
	case BC5_SNorm:
	case BC6H_Typeless:
	case BC6H_Uf16:
	case BC6H_Sf16:
	case BC7_Typeless:
	case BC7_UNorm:
	case BC7_UNorm_sRGB:
		return true;

	default:
		return false;
	}
}

}
