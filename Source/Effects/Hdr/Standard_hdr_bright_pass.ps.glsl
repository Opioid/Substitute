void main() 
{
	vec3 color = texture(g_bloom_map, pixel_in.tex_coord).rgb;

	color = tonemap(color, 4.f, pixel_in.tex_coord);	
	
	pixel_out.color = color;
}