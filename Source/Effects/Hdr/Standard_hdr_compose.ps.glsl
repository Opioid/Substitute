void main() 
{
	ivec2 pixel_coord = ivec2(gl_FragCoord.x, gl_FragCoord.y);

	vec3 color = texelFetch(g_hdr_map, pixel_coord, 0).rgb;

	vec3 tonemapped_color = tonemap(color, 0.f, pixel_in.tex_coord);

	// Sample the bloom
	vec3 bloom = texture(g_bloom_map, pixel_in.tex_coord).rgb;

	// Add in the bloom
	tonemapped_color = /*0.000001f **/ tonemapped_color + bloom;	

	pixel_out.color = vec4(tonemapped_color, sqrt(luminance(tonemapped_color))); 
}