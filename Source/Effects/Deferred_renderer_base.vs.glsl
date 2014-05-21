void main() 
{ 
	vec4 position = vec4(vertex_in.position, 1.f);
	gl_Position = g_view_projection * g_world * position; 
	
	vertex_out.tex_coord = vertex_in.tex_coord;

	mat4 world_view = g_view * g_world;

	vertex_out.normal_vs = mat3(world_view) * (2.f * (vertex_in.normal.xyz - 0.5f));

#ifdef TANGENT_SPACE
	vec3 position_vs = (world_view * position).xyz;

	vertex_out.vertex_to_eye_vs = -position_vs; 
#endif
}