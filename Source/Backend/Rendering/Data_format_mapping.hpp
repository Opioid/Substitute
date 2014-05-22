#pragma once

#include "Rendering/Data_format.hpp"
#include <cstdint>

namespace rendering
{

struct Data_format_mapping
{
	Data_format_mapping(int internal_format = 0, int format = 0, int num_elemts = 0, uint32_t type = 0, bool normalized = false);

	int		internal_format;
	int		format;
	int 		num_elements_;
	uint32_t 	type;
	unsigned char 	normalized;

	static Data_format_mapping map(Data_format::Value format);
};

}


