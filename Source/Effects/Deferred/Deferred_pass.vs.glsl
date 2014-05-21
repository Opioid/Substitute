void main()
{
#ifdef VOLUME

	vec4 position = vec4(vertex_in.position, 1.f);
	
	gl_Position = g_view_projection * g_world * position; 

	vertex_out.view_ray = (g_view * g_world * position).xyz; 

#else

	gl_Position = vec4(vertex_in.position.x, vertex_in.position.y, 1.f, 1.f);

	vertex_out.view_ray = g_view_rays[gl_VertexID].xyz;
	
#endif
}