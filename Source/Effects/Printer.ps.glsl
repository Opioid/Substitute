void main() 
{
#ifdef TEXTURE
	pixel_out.color = texture(g_color_map, pixel_in.tex_coord) * pixel_in.color;
#elif defined FONT
	pixel_out.color = vec4(pixel_in.color.rgb, pixel_in.color.a * pow(texture(g_font_map, pixel_in.tex_coord).r, 1.f / 1.5f));
#else
	pixel_out.color = pixel_in.color;
#endif
}