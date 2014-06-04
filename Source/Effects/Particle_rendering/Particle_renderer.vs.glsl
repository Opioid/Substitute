void main()
{
	vec3 position = lerp(vertex_in.previous_position, vertex_in.current_position, g_time.x);
	vec4 properties = lerp(vertex_in.previous_properties, vertex_in.current_properties, g_time.x);

	vertex_out.position = position;//(g_view * vec4(position, 1.f)).xyz;
	vertex_out.properties = properties;
}