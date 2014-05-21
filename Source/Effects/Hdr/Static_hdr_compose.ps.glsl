void main() 
{
	ivec2 pixel_coord = ivec2(gl_FragCoord.x, gl_FragCoord.y);

	vec3 color = texelFetch(g_hdr_map, pixel_coord, 0).rgb;

	vec3 tonemapped_color = tonemap(color, 0.f);

	pixel_out.color = vec4(tonemapped_color, sqrt(luminance(tonemapped_color))); 
}