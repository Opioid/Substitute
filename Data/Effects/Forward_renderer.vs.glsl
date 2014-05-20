void main() 
{ 
	vec4 position = vec4(vertex_in.position, 1.f);
	gl_Position = g_world_view_projection * position; 
	
	vertex_out.tex_coord = vertex_in.tex_coord;

	vertex_out.normal_vs =  mat3(g_world_view) * (2.f * (vertex_in.normal.xyz - 0.5f));

#ifdef TANGENT_SPACE
	vertex_out.vertex_to_eye_vs = -(g_world_view * position).xyz; 
#endif
}