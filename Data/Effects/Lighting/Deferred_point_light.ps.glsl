float light_falloff(float radius, vec3 v)
{
	float squared_dist = dot(v, v);
	float dist = sqrt(squared_dist);

	float a  = dist / radius;
	float a4 = a * a * a * a;
	float b = saturate(1.f - a4);

	float num = b * b;

	float denom = squared_dist + 1.f;

	return num / denom;
}

void main() 
{
	ivec2 pixel_coord = ivec2(gl_FragCoord.x, gl_FragCoord.y);

	float depth = texelFetch(g_depth_map, pixel_coord, 0).r;
	vec3 normal = decode_normal_spheremap(texelFetch(g_normal_map, pixel_coord, 0).xy);
	vec3 color = texelFetch(g_color_map, pixel_coord, 0).rgb;
	vec2 metallic_and_roughness = texelFetch(g_surface_map, pixel_coord, 0).xy;
	
	vec3 position_vs = get_position_vs(pixel_in.view_ray, depth);

	vec3 light_vector = g_light_position_vs - position_vs;
	vec3 light_dir = normalize(light_vector);

	vec3 light = lighting(position_vs, normal, light_dir, color, metallic_and_roughness.x, metallic_and_roughness.y);

	vec4 light_energy_and_range = g_light_data[0];

	pixel_out.color = light * light_falloff(light_energy_and_range.w, light_vector) * light_energy_and_range.xyz; 
}