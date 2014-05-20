#include "Font_factory.hpp"
#include "Font_descriptor.hpp"
#include <algorithm>
#include "ft2build.h"

#include FT_FREETYPE_H

namespace fonts
{

Font_factory::Font_factory() : m_ft_lib(nullptr)
{}

bool Font_factory::init()
{
	m_glyph_order.resize(192);

	return FT_Init_FreeType(&m_ft_lib) == 0;
}

void Font_factory::release()
{
	FT_Done_FreeType(m_ft_lib);
}

bool Font_factory::fill_font_descriptor(Font_descriptor* desc, const std::string& name, uint32_t width, uint32_t height)
{
	FT_Error error;

	FT_Face face;
	error = FT_New_Face(m_ft_lib, name.c_str(), 0, &face);

	if (error)
	{
		return false;
	}

	FT_Set_Pixel_Sizes(face, width, height);

	int buffer_width;
	int buffer_height;
	int max_glyph_height;
	calculate_buffer_dimension_and_glyph_order(buffer_width, buffer_height, max_glyph_height, face);

//	buffer_width = 256;
//	buffer_height = 211;

	desc->face_data = new unsigned char[buffer_width * buffer_height];

	int pencil_x = 0;
	int pencil_y = 0;
	int max_glyph_height_in_last_row = 0;

	for (uint32_t c = 0; c < m_glyph_order.size(); ++c)
	{
		FT_Load_Glyph(face, FT_Get_Char_Index(face, m_glyph_order[c].id), 0);
		FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

		FT_Bitmap* bitmap = &face->glyph->bitmap;

		if (pencil_x + bitmap->width >= buffer_width)
		{
			pencil_x = 0;
			pencil_y += max_glyph_height_in_last_row;
			max_glyph_height_in_last_row = 0;
		}

		copy(bitmap->buffer, desc->face_data, pencil_x, pencil_y, bitmap->width, bitmap->rows, buffer_width);

		const uint32_t g = Font_descriptor::s_character_mappings[m_glyph_order[c].id];

		desc->glyphs[g].offset_x = face->glyph->bitmap_left;
		desc->glyphs[g].offset_y = max_glyph_height - face->glyph->bitmap_top;

		desc->glyphs[g].start_x = pencil_x;
		desc->glyphs[g].start_y = pencil_y;

		desc->glyphs[g].width  = bitmap->width;
		desc->glyphs[g].height = bitmap->rows;

		desc->glyphs[g].advance = face->glyph->advance.x / 64;

		pencil_x += bitmap->width;

		if (max_glyph_height_in_last_row < bitmap->rows)
		{
			max_glyph_height_in_last_row = bitmap->rows;
		}
	}
		
	FT_Load_Glyph(face, FT_Get_Char_Index(face, '\t'), 0);
	desc->tab_width = face->glyph->advance.x / 64;
	desc->line_height = (face->size->metrics.ascender - face->size->metrics.descender) / 64;

	desc->width = buffer_width;
	desc->height = buffer_height;

	// release the font resources
	FT_Done_Face(face);

	return true;
}

void Font_factory::calculate_buffer_dimension_and_glyph_order(int &width, int &height, int &max_glyph_height, FT_Face face)
{
	int max_glyph_width = 0;

	for (uint32_t c = 32, i = 0; c < 256; ++c, ++i)
	{
		if (c == 128)
		{
			c += 32;
		}

		FT_Load_Glyph(face, FT_Get_Char_Index(face, c), 0);

		const int w = face->glyph->metrics.width / 64;
		if (max_glyph_width < w)
		{
			max_glyph_width = w;
		}

		m_glyph_order[i].id = c;
		m_glyph_order[i].height = face->glyph->metrics.height / 64;
	}
		
	std::sort(m_glyph_order.begin(), m_glyph_order.end(), [] (const Glyph_dimensions& a, const Glyph_dimensions& b) -> bool
							       {
									if (a.height == b.height)
									{
										return a.id > b.id;
									}
									else
									{
										return a.height > b.height;
									}
								} );
															
	width = max_glyph_width * 9;

	int max_glyph_height_in_last_row = 0;
	int pencil_x = 0;
	height = 0;
	max_glyph_height = 0;

	for (uint32_t c = 0; c < m_glyph_order.size(); ++c)
	{
		FT_Load_Glyph(face, FT_Get_Char_Index(face, m_glyph_order[c].id), 0);

		const int w = face->glyph->metrics.width / 64;
			
		if (pencil_x + w >= width)
		{
			pencil_x = 0;
			height += max_glyph_height_in_last_row;
			max_glyph_height_in_last_row = 0;
		}

		pencil_x += w;

		const int h = face->glyph->metrics.height / 64;

		if (max_glyph_height_in_last_row < h)
		{
			max_glyph_height_in_last_row = h;
		}

		if (max_glyph_height < h)
		{
			max_glyph_height = h;
		}
	}

	height += max_glyph_height_in_last_row;
}

void Font_factory::copy(unsigned char* source, unsigned char* dest, int x, int y, int width, int height, int dest_width)
{
	const int dif = dest_width - width;

	dest += dest_width * y + x;

	for (int i = 0; i < height; ++i, dest += dif)
	{
		for (int j = 0; j < width; ++j, ++dest, ++source)
		{
			*dest = *source;
		}
	}
}

}
