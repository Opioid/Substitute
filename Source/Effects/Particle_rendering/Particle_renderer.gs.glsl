const vec2 g_offsets[] = { vec2(-1.f, 1.f), vec2(1.f, 1.f), vec2(-1.f, -1.f), vec2(1.f, -1.f) };

void main()
{
	vec2 scale = geometry_in[0].properties.xy;

	float angle = geometry_in[0].position.w;
	float c = cos(angle);
	float s = sin(angle);

	mat2 rotation = mat2(scale.x * c, scale.x * -s, 
						 scale.y * s, scale.y *  c);

	vec3 position_vs = (g_view * vec4(geometry_in[0].position.xyz, 1.f)).xyz;

	vec4 corners[4];
	corners[0] = vec4(position_vs.xy + rotation * g_offsets[0], position_vs.z, 1.f);
	corners[1] = vec4(position_vs.xy + rotation * g_offsets[1], position_vs.z, 1.f);
	corners[2] = vec4(position_vs.xy + rotation * g_offsets[2], position_vs.z, 1.f);
	corners[3] = vec4(position_vs.xy + rotation * g_offsets[3], position_vs.z, 1.f);


#ifdef LIGHTING
	vec3 transformed_position = ((g_light_transformation * vec4(position_vs, 1.f)).xyz + 1.f) * 0.5f;

	geometry_out.cube_0 = texture(g_irradiance_volume_map0, transformed_position);
	geometry_out.cube_1 = texture(g_irradiance_volume_map1, transformed_position);
	geometry_out.cube_2 = texture(g_irradiance_volume_map2, transformed_position);
	geometry_out.cube_3 = texture(g_irradiance_volume_map3, transformed_position);
	geometry_out.cube_4 = texture(g_irradiance_volume_map4, transformed_position);
#endif // LIGHTING

#ifdef SOFT
	geometry_out.depth = position_vs.z;
#endif // SOFT


	gl_Position = g_projection * corners[0];
#ifdef ARRAY
	geometry_out.tex_coord = vec4(0.f, 0.f, geometry_in[0].properties.zw);
#else
	geometry_out.tex_coord = vec3(0.f, 0.f, geometry_in[0].properties.w);
#endif // ARRAY
#ifdef LIGHTING
	geometry_out.normal_vs = normalize(vec3(corners[0].xy - position_vs.xy, -0.25f));
#endif // LIGHTING
	EmitVertex();

	gl_Position = g_projection * corners[1];
#ifdef ARRAY
	geometry_out.tex_coord = vec4(1.f, 0.f, geometry_in[0].properties.zw);
#else
	geometry_out.tex_coord = vec3(1.f, 0.f, geometry_in[0].properties.w);
#endif // ARRAY	
#ifdef LIGHTING
	geometry_out.normal_vs = normalize(vec3(corners[1].xy - position_vs.xy, -0.25f));
#endif // LIGHTING
	EmitVertex();

	gl_Position = g_projection * corners[2];
#ifdef ARRAY
	geometry_out.tex_coord = vec4(0.f, 1.f, geometry_in[0].properties.zw);
#else
	geometry_out.tex_coord = vec3(0.f, 1.f, geometry_in[0].properties.w);
#endif // ARRAY
#ifdef LIGHTING
	geometry_out.normal_vs = normalize(vec3(corners[2].xy - position_vs.xy, -0.25f));
#endif // LIGHTING
	EmitVertex();

	gl_Position = g_projection * corners[3];
#ifdef ARRAY
	geometry_out.tex_coord = vec4(1.f, 1.f, geometry_in[0].properties.zw);
#else
	geometry_out.tex_coord = vec3(1.f, 1.f, geometry_in[0].properties.w);
#endif // ARRAY
#ifdef LIGHTING
	geometry_out.normal_vs = normalize(vec3(corners[3].xy - position_vs.xy, -0.25f));
#endif // LIGHTING
	EmitVertex();

	EndPrimitive();
}