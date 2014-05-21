void main() 
{ 
	gl_Position = g_view_projection * g_world * vec4(vertex_in.position, 1.f); 

	vertex_out.normal = mat3(g_view * g_world) * vertex_in.normal;
}