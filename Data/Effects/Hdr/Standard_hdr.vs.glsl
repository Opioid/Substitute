void main()
{
	gl_Position = vec4(vertex_in.position.x, vertex_in.position.y, 1.f, 1.f);

	vertex_out.tex_coord = vertex_in.tex_coord;
}