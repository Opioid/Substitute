#pragma once

#include <cstdint>

namespace fonts
{

struct Font_descriptor
{

	Font_descriptor();
	~Font_descriptor();


	struct Glyph_descriptor
	{
		int offset_x, offset_y;
		uint32_t start_x, start_y;
		uint32_t width, height;
			
		uint32_t advance;
	};


	Glyph_descriptor glyphs[192];

	uint32_t width;
	uint32_t height;
	unsigned char* face_data;

	uint32_t tab_width;
	uint32_t line_height;

	static const uint32_t s_character_mappings[];
};

}
