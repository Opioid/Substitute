float soft_fade(float scene_depth, float particle_depth)
{
	return saturate((scene_depth - particle_depth) * 1.5f);
}

/*
float soft_fade(float scene_depth, float particle_depth)
{
	float d = scene_depth - particle_depth;
	
	float fade = 0.5f * pow(saturate(2.f * ((d > 0.5) ? 1.f - d : d)), 0.4f);
	return (d > 0.5) ? 1.f - fade : fade;	
}
*/

vec3 ambient_cube_lighting(vec3 normal_ws, vec3 ambient_cube[6])
{
	vec3 normal_squared = normal_ws * normal_ws; 
	
	int is_negative_x = normal_ws.x < 0.f ? 1 : 0;
	int is_negative_y = normal_ws.y < 0.f ? 1 : 0;
	int is_negative_z = normal_ws.z < 0.f ? 1 : 0;
	
	return normal_squared.x * ambient_cube[is_negative_x]     
		 + normal_squared.y * ambient_cube[is_negative_y + 2] 
		 + normal_squared.z * ambient_cube[is_negative_z + 4]; 
}

vec3 two_sided_ambient_cube_lighting(vec3 normal_ws, float translucency, vec3 ambient_cube[6])
{
	vec3 normal_squared = normal_ws * normal_ws; 
	
	int is_negative_x = normal_ws.x < 0.f ? 1 : 0;
	int is_negative_y = normal_ws.y < 0.f ? 1 : 0;
	int is_negative_z = normal_ws.z < 0.f ? 1 : 0;
	
	int is_positive_x = -normal_ws.x < 0.f ? 1 : 0;
	int is_positive_y = -normal_ws.y < 0.f ? 1 : 0;
	int is_positive_z = -normal_ws.z < 0.f ? 1 : 0;

	return normal_squared.x * (ambient_cube[is_negative_x]     + translucency * ambient_cube[is_positive_x])
		 + normal_squared.y * (ambient_cube[is_negative_y + 2] + translucency * ambient_cube[is_positive_y + 2])
		 + normal_squared.z * (ambient_cube[is_negative_z + 4] + translucency * ambient_cube[is_positive_z + 4]); 
}

void main()
{
#ifdef ARRAY
	vec4 color = texture(g_color_map_array, vec3(pixel_in.tex_coord.xyz));

	float alpha = pixel_in.tex_coord.w;
#else
	vec4 color = texture(g_color_map, pixel_in.tex_coord.xy);

	float alpha = pixel_in.tex_coord.z;
#endif // ARRAY	

#ifdef SOFT
	ivec2 pixel_coord = ivec2(gl_FragCoord.x, gl_FragCoord.y);

	float depth = texelFetch(g_depth_map, pixel_coord, 0).r;

	float linear_depth = calculate_linear_depth(depth, vec2(g_view_rays[0].w, g_view_rays[1].w));

	float fade = soft_fade(linear_depth, pixel_in.depth);

	color.a *= fade;
#endif // SOFT

	color.a *= alpha;

#ifdef LIGHTING

	vec3 ambient_cube[6] = { pixel_in.cube_0.xyz,
							 vec3(pixel_in.cube_0.w,  pixel_in.cube_1.xy),
							 vec3(pixel_in.cube_1.zw, pixel_in.cube_2.x),
							 pixel_in.cube_2.yzw,
							 pixel_in.cube_3.xyz,
							 vec3(pixel_in.cube_3.w, pixel_in.cube_4.xy) };

	vec3 normal_vs = normalize(pixel_in.normal_vs);

	vec3 normal_ws = (vec4(normal_vs, 0.f) * g_view).xyz;						 

	/*
	float translucency = 1.f - color.a;

	vec3 lighting = two_sided_ambient_cube_lighting(normal_ws, translucency, ambient_cube);
	*/

	vec3 lighting = ambient_cube_lighting(normal_ws, ambient_cube);

	color.rgb = lighting * color.rgb;

#endif // LIGHTING

	pixel_out.color = color;
}