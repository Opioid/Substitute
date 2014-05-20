#pragma once

#include <string>
#include <vector>
#include <cstdint>

struct FT_LibraryRec_;
struct FT_FaceRec_;

typedef FT_LibraryRec_ *FT_Library;
typedef FT_FaceRec_ *FT_Face;

namespace fonts
{

struct Font_descriptor;


class Font_factory
{

public:

	Font_factory();

	bool init();
	void release();

	bool fill_font_descriptor(Font_descriptor* desc, const std::string& name, uint32_t width, uint32_t height);

private:

	void calculate_buffer_dimension_and_glyph_order(int &width, int &height, int &max_glyph_height, FT_Face face);

	static void copy(unsigned char* source, unsigned char* dest, int x, int y, int width, int height, int dest_width);

	FT_Library m_ft_lib;

	struct Glyph_dimensions
	{
		uint32_t id;
		int      height;
	};

	std::vector<Glyph_dimensions> m_glyph_order;
};

}
