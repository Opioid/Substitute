void main() 
{
	vec3 color = vec3(0.f, 0.f, 0.f);

	color += 0.1875f * texture(g_bloom_map_point, pixel_in.tex_coord).rgb;

#ifdef HORIZONTAL

	color += 0.171875f * texture(g_bloom_map_point, pixel_in.tex_coord + vec2(g_texel_size.x, 0.f)).rgb;
	color += 0.171875f * texture(g_bloom_map_point, pixel_in.tex_coord - vec2(g_texel_size.x, 0.f)).rgb;
	
	color += 0.140625f * texture(g_bloom_map_point, pixel_in.tex_coord + vec2(2.f * g_texel_size.x, 0.f)).rgb;
	color += 0.140625f * texture(g_bloom_map_point, pixel_in.tex_coord - vec2(2.f * g_texel_size.x, 0.f)).rgb;
	
	color += 0.09375f * texture(g_bloom_map_point, pixel_in.tex_coord + vec2(3.f * g_texel_size.x, 0.f)).rgb;
	color += 0.09375f * texture(g_bloom_map_point, pixel_in.tex_coord - vec2(3.f * g_texel_size.x, 0.f)).rgb;

#else

	color += 0.171875f * texture(g_bloom_map_point, pixel_in.tex_coord + vec2(0.f, g_texel_size.x)).rgb;
	color += 0.171875f * texture(g_bloom_map_point, pixel_in.tex_coord - vec2(0.f, g_texel_size.x)).rgb;
	
	color += 0.140625f * texture(g_bloom_map_point, pixel_in.tex_coord + vec2(0.f, 2.f * g_texel_size.x)).rgb;
	color += 0.140625f * texture(g_bloom_map_point, pixel_in.tex_coord - vec2(0.f, 2.f * g_texel_size.x)).rgb;
	
	color += 0.09375f * texture(g_bloom_map_point, pixel_in.tex_coord + vec2(0.f, 3.f * g_texel_size.x)).rgb;
	color += 0.09375f * texture(g_bloom_map_point, pixel_in.tex_coord - vec2(0.f, 3.f * g_texel_size.x)).rgb;

#endif

	pixel_out.color = color;
}