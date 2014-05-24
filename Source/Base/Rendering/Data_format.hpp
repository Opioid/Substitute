#pragma once

#include <cstdint>

namespace rendering
{

struct Data_format
{
	enum Value
	{
		Unknown                     = 0,
		R32G32B32A32_Typeless       = 1,
		R32G32B32A32_Float          = 2,
		R32G32B32A32_UInt           = 3,
		R32G32B32A32_SInt           = 4,
		R32G32B32_Typeless          = 5,
		R32G32B32_Float             = 6,
		R32G32B32_UInt              = 7,
		R32G32B32_SInt              = 8,
		R16G16B16A16_Typeless       = 9,
		R16G16B16A16_Float          = 10,
		R16G16B16A16_UNorm          = 11,
		R16G16B16A16_UInt           = 12,
		R16G16B16A16_SNorm          = 13,
		R16G16B16A16_SInt           = 14,
		R32G32_Typeless             = 15,
		R32G32_Float                = 16,
		R32G32_UInt                 = 17,
		R32G32_SInt                 = 18,
		R32G8X24_Typeless           = 19,
		D32_Float_S8X24_UInt        = 20,
		R32_Float_X8X24_Typeless    = 21,
		X32_Typeless_G8X24_UInt     = 22,
		R10G10B10A2_Typeless        = 23,
		R10G10B10A2_UNorm           = 24,
		R10G10B10A2_UInt            = 25,
		R11G11B10_Float             = 26,
		R8G8B8A8_Typeless           = 27,
		R8G8B8A8_UNorm              = 28,
		R8G8B8A8_UNorm_sRGB         = 29,
		R8G8B8A8_UInt               = 30,
		R8G8B8A8_SNorm              = 31,
		R8G8B8A8_SInt               = 32,
		R16G16_Typeless             = 33,
		R16G16_Float                = 34,
		R16G16_UNorm                = 35,
		R16G16_UInt                 = 36,
		R16G16_SNorm                = 37,
		R16G16_SInt                 = 38,
		R32_Typeless                = 39,
		D32_Float                   = 40,
		R32_Float                   = 41,
		R32_UInt                    = 42,
		R32_SInt                    = 43,
		R24G8_Typeless              = 44,
		D24_UNorm_S8_UInt           = 45,
		R24_UNorm_X8_Typeless       = 46,
		X24_Typeless_G8_UInt        = 47,
		R8G8_Typeless               = 48,
		R8G8_UNorm                  = 49,
		R8G8_UInt                   = 50,
		R8G8_SNorm                  = 51,
		R8G8_SInt                   = 52,
		R16_Typeless                = 53,
		R16_Float                   = 54,
		D16_UNorm                   = 55,
		R16_UNorm                   = 56,
		R16_UInt                    = 57,
		R16_SNorm                   = 58,
		R16_SInt                    = 59,
		R8_Typeless                 = 60,
		R8_UNorm                    = 61,
		R8_UInt                     = 62,
		R8_SNorm                    = 63,
		R8_SInt                     = 64,
		A8_UNorm                    = 65,
		R1_UNorm                    = 66,
		R9G9B9E5_SharedExp          = 67,
		R8G8_B8G8_UNorm             = 68,
		G8R8_G8B8_UNorm             = 69,
		BC1_Typeless                = 70,
		BC1_UNorm                   = 71,
		BC1_UNorm_sRGB              = 72,
		BC2_Typeless                = 73,
		BC2_UNorm                   = 74,
		BC2_UNorm_sRGB              = 75,
		BC3_Typeless                = 76,
		BC3_UNorm                   = 77,
		BC3_UNorm_sRGB              = 78,
		BC4_Typeless                = 79,
		BC4_UNorm                   = 80,
		BC4_SNorm                   = 81,
		BC5_Typeless                = 82,
		BC5_UNorm                   = 83,
		BC5_SNorm                   = 84,
		B5G6R5_UNorm                = 85,
		B5G5R5A1_UNorm              = 86,
		B8G8R8A8_UNorm              = 87,
		B8G8R8X8_UNorm              = 88,
		R10G10B10_Xr_Bias_A2_UNorm  = 89,
		B8G8R8A8_Typeless           = 90,
		B8G8R8A8_UNorm_sRGB         = 91,
		B8G8R8X8_Typeless           = 92,
		B8G8R8X8_UNorm_sRGB         = 93,
		BC6H_Typeless               = 94,
		BC6H_Uf16                   = 95,
		BC6H_Sf16                   = 96,
		BC7_Typeless                = 97,
		BC7_UNorm                   = 98,
		BC7_UNorm_sRGB              = 99,
		Force_UInt                  = 0xffffffff
	};

	static bool  is_sRGB(Value format);

	static Value to_sRGB(Value format);

	static uint32_t num_elements_per_block(Value format);

	static uint32_t num_bytes_per_block(Value format);

	static bool is_compressed(Value format);
};

}


