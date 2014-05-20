const vec3 g_luminance_vector = vec3(0.299f, 0.587f, 0.114f);

float luminance(vec3 color)
{
	return dot(color, g_luminance_vector);
}

void main() 
{
	ivec2 pixel_coord = ivec2(gl_FragCoord.x, gl_FragCoord.y);

	vec3 color = texelFetch(g_hdr_map, pixel_coord, 0).rgb;

	pixel_out.color = vec4(color, sqrt(luminance(color))); 
}