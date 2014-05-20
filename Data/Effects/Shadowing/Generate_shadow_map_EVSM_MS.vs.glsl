void main() 
{ 
	vec4 position = vec4(vertex_in.position, 1.f);

	vec4 out_position = g_view_projection * g_world * position;

	gl_Position = out_position; 

#ifdef ALPHA_TEST

	vertex_out.tex_coord = vertex_in.tex_coord;	

#endif
}