vec3 evaluate_ambient_cube(vec3 position_vs, vec3 normal_ws) 
{ 
	vec3 transformed_position = ((g_light_data * vec4(position_vs, 1.f)).xyz + 1.f) * 0.5f;

/*
	vec3 ambient_cube[6] = { texture(g_irradiance_volume_map0, transformed_position).rgb,
							 texture(g_irradiance_volume_map1, transformed_position).rgb,
							 texture(g_irradiance_volume_map2, transformed_position).rgb,
							 texture(g_irradiance_volume_map3, transformed_position).rgb,
							 texture(g_irradiance_volume_map4, transformed_position).rgb,
							 texture(g_irradiance_volume_map5, transformed_position).rgb };
*/

	vec4 cube0 = texture(g_irradiance_volume_map0, transformed_position);
	vec4 cube1 = texture(g_irradiance_volume_map1, transformed_position);
	vec4 cube2 = texture(g_irradiance_volume_map2, transformed_position);
	vec4 cube3 = texture(g_irradiance_volume_map3, transformed_position);
	vec4 cube4 = texture(g_irradiance_volume_map4, transformed_position);

	vec3 ambient_cube[6] = { cube0.xyz,
							 vec3(cube0.w, cube1.xy),
							 vec3(cube1.zw, cube2.x),
							 cube2.yzw,
							 cube3.xyz,
							 vec3(cube3.w, cube4.xy) };

	vec3 normal_squared = normal_ws * normal_ws; 
	
	int is_negative_x = normal_ws.x < 0.f ? 1 : 0;
	int is_negative_y = normal_ws.y < 0.f ? 1 : 0;
	int is_negative_z = normal_ws.z < 0.f ? 1 : 0;
	
	vec3 color = normal_squared.x * ambient_cube[is_negative_x]     
		       + normal_squared.y * ambient_cube[is_negative_y + 2] 
		       + normal_squared.z * ambient_cube[is_negative_z + 4]; 
			  
    return color; 
} 