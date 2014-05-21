void main() 
{ 
	gl_Position = g_view_projection * g_world * vec4(vertex_in.position, 1.f); 
	
	vertex_out.color = vertex_in.color;
}