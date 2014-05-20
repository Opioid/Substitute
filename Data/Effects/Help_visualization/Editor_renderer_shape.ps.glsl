void main()
{
    vec3 normal = normalize(pixel_in.normal);
	
	pixel_out.color = g_color * dot(normal, vec3(0.f, 0.f, -1.f)) + 0.25f * g_color;
}