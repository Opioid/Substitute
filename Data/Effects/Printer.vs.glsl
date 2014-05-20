void main()
{
	vertex_out.position  = vertex_in.position * g_1_div_half_res.xyxy + vec4(-1.f, 1.f, 0.f, 0.f);
	vertex_out.tex_coord = vertex_in.tex_coord;
	vertex_out.color     = vertex_in.color;
}