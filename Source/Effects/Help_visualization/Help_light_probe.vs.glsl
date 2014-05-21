void main() 
{ 
	vec4 position = vec4(vertex_in.position, 1.f);
	gl_Position = g_view_projection * g_world * position; 

	vertex_out.position = g_position_vs;

	vertex_out.normal = 2.f * (vertex_in.normal.xyz - 0.5f);
}