const vec2 g_offsets[] = { vec2(-1.f, 1.f), vec2(1.f, 1.f), vec2(-1.f, -1.f), vec2(1.f, -1.f) };

void main()
{
	float scale = geometry_in[0].properties.x;

	float angle = geometry_in[0].properties.y;
	float c = cos(angle);
	float s = sin(angle);

	mat2 rotation = scale * mat2(c, -s, s, c);

	vec3 postions_vs = (g_view * vec4(geometry_in[0].position, 1.f)).xyz;

	vec4 corners[4];
	corners[0] = vec4(postions_vs.xy + rotation * g_offsets[0], postions_vs.z, 1.f);
	corners[1] = vec4(postions_vs.xy + rotation * g_offsets[1], postions_vs.z, 1.f);
	corners[2] = vec4(postions_vs.xy + rotation * g_offsets[2], postions_vs.z, 1.f);
	corners[3] = vec4(postions_vs.xy + rotation * g_offsets[3], postions_vs.z, 1.f);

	gl_Position = g_projection * corners[0];
	geometry_out.tex_coord_and_factor = vec3(0.f, 0.f, geometry_in[0].properties.z);
#ifdef SOFT
	geometry_out.depth = postions_vs.z;
#endif // SOFT
	EmitVertex();

	gl_Position = g_projection * corners[1];
	geometry_out.tex_coord_and_factor = vec3(1.f, 0.f, geometry_in[0].properties.z);
#ifdef SOFT
	geometry_out.depth = postions_vs.z;
#endif // SOFT
	EmitVertex();

	gl_Position = g_projection * corners[2];
	geometry_out.tex_coord_and_factor = vec3(0.f, 1.f, geometry_in[0].properties.z);
#ifdef SOFT
	geometry_out.depth = postions_vs.z;
#endif // SOFT
	EmitVertex();

	gl_Position = g_projection * corners[3];
	geometry_out.tex_coord_and_factor = vec3(1.f, 1.f, geometry_in[0].properties.z);
#ifdef SOFT
	geometry_out.depth = postions_vs.z;
#endif // SOFT
	EmitVertex();

	EndPrimitive();
}