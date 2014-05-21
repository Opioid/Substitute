void main()
{
	vec4 corners[4];

	corners[0] = vec4(geometry_in[0].position.x,  geometry_in[0].position.y, 1.f, 1.f);
	corners[1] = vec4(geometry_in[0].position.x + geometry_in[0].position.z, geometry_in[0].position.y, 1.f, 1.f);
	corners[2] = vec4(geometry_in[0].position.x,  geometry_in[0].position.y + geometry_in[0].position.w, 1.f, 1.f);
	corners[3] = vec4(corners[1].x, corners[2].y, 1.f, 1.f);

	gl_Position  = corners[0];
	geometry_out.tex_coord = geometry_in[0].tex_coord.xy;
	geometry_out.color     = geometry_in[0].color;
	EmitVertex();

	gl_Position  = corners[1];
	geometry_out.tex_coord = geometry_in[0].tex_coord.zy;
	geometry_out.color     = geometry_in[0].color;
	EmitVertex();

	gl_Position  = corners[2];
	geometry_out.tex_coord = geometry_in[0].tex_coord.xw;
	geometry_out.color     = geometry_in[0].color;
	EmitVertex();

	gl_Position  = corners[3];
	geometry_out.tex_coord = geometry_in[0].tex_coord.zw;
	geometry_out.color     = geometry_in[0].color;
	EmitVertex();

	EndPrimitive();
}