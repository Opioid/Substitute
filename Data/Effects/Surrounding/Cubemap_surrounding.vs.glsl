void main()
{
	gl_Position = vec4(vertex_in.position.x, vertex_in.position.y, 1.f, 1.f);

	vertex_out.view_ray_ws = g_view_rays_ws[gl_VertexID].xyz;
}