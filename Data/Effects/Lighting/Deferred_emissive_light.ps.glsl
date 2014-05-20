void main() 
{
	ivec2 pixel_coord = ivec2(gl_FragCoord.x, gl_FragCoord.y);

	vec4 color_and_emissive_factor = texelFetch(g_color_map, pixel_coord, 0);

	pixel_out.color = color_and_emissive_factor.rgb * color_and_emissive_factor.a * 100.f; 
}