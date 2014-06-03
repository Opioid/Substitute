void main()
{
	vec4 corners[4];

	float scale = geometry_in[0].properties.x;

	corners[0] = vec4(geometry_in[0].position.x - scale, geometry_in[0].position.y + scale, geometry_in[0].position.z, 1.f);
	corners[1] = vec4(geometry_in[0].position.x + scale, geometry_in[0].position.y + scale, geometry_in[0].position.z, 1.f);
	corners[2] = vec4(corners[0].x,                      geometry_in[0].position.y - scale, geometry_in[0].position.z, 1.f);
	corners[3] = vec4(corners[1].x,                      corners[2].y,    					geometry_in[0].position.z, 1.f);

	gl_Position  = g_projection * corners[0];
	geometry_out.tex_coord_and_factor = vec3(0.f, 0.f, geometry_in[0].properties.z);
	EmitVertex();

	gl_Position  = g_projection * corners[1];
	geometry_out.tex_coord_and_factor = vec3(1.f, 0.f, geometry_in[0].properties.z);
	EmitVertex();

	gl_Position  = g_projection * corners[2];
	geometry_out.tex_coord_and_factor = vec3(0.f, 1.f, geometry_in[0].properties.z);
	EmitVertex();

	gl_Position  = g_projection * corners[3];
	geometry_out.tex_coord_and_factor = vec3(1.f, 1.f, geometry_in[0].properties.z);
	EmitVertex();

	EndPrimitive();
}