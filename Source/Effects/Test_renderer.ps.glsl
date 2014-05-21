void main() 
{
	pixel_out.color = texture(g_color_map, pixel_in.tex_coord).rgb;
}