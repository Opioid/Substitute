const vec3 g_luminance_vector = vec3(0.299f, 0.587f, 0.114f);

float luminance(vec3 color)
{
	return dot(color, g_luminance_vector);
}

void main() 
{
	vec3 color = texture(g_hdr_map, pixel_in.tex_coord).rgb;

	pixel_out.luminance = log(clamp(luminance(color), 0.0001f, 4084.f));
}