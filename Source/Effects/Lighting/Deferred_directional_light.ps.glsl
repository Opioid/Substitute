void main() 
{
	ivec2 pixel_coord = ivec2(gl_FragCoord.x, gl_FragCoord.y);

	float depth = texelFetch(g_depth_map, pixel_coord, 0).r;
	vec3 normal = decode_normal_spheremap(texelFetch(g_normal_map, pixel_coord, 0).xy);
	vec3 color = texelFetch(g_color_map, pixel_coord, 0).rgb;
	vec2 metallic_and_roughness = texelFetch(g_surface_map, pixel_coord, 0).xy;
	
	vec3 position_vs = get_position_vs(pixel_in.view_ray, depth);

	vec3 light_direction_vs = g_light_data[1].xyz;

	vec3 light = lighting(position_vs, normal, light_direction_vs, color, metallic_and_roughness.x, metallic_and_roughness.y);

	vec3 light_energy = g_light_data[0].xyz;

	pixel_out.color = light * light_energy; 
}