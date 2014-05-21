void main() 
{ 
	vec4 position = vec4(vertex_in.position, 1.f);
	gl_Position = g_world_view_projection * position; 
	
	vertex_out.tex_coord = vertex_in.tex_coord;
}