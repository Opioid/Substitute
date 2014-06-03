void main()
{
	vec4 color = texture(g_color_map, pixel_in.tex_coord_and_factor.xy);

	pixel_out.color = pixel_in.tex_coord_and_factor.z * color;
}