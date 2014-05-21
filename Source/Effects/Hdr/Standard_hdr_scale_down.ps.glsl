void main() 
{
	vec3 color = vec3(0.f, 0.f, 0.f);

	for (uint i = 0; i < 4; ++i)
	{
		color += g_filter_kernel[i].z * texture(g_hdr_map, pixel_in.tex_coord + g_filter_kernel[i].xy).xyz; 
	}

//	vec3 color = texture(g_hdr_map, pixel_in.tex_coord + 0.00001 * g_filter_kernel[0].xy).xyz; 

	color = tonemap(color, 6.f, pixel_in.tex_coord);

	pixel_out.color = color;
}