void main() 
{
	ivec2 pixel_coord = ivec2(gl_FragCoord.x, gl_FragCoord.y);

	float depth = texelFetch(g_depth_map, pixel_coord, 0).r;
	vec3 normal_vs = decode_normal_spheremap(texelFetch(g_normal_map, pixel_coord, 0).xy);
	vec3 color = texelFetch(g_color_map, pixel_coord, 0).rgb;
	vec3 surface = texelFetch(g_surface_map, pixel_coord, 0).xyz;

	float metallic = surface.x;
	float cavity   = surface.z;

	vec3 position_vs = get_position_vs(pixel_in.view_ray, depth, vec2(g_view_rays[0].w, g_view_rays[1].w));

	vec3 normal_ws = (g_inverse_view * vec4(normal_vs, 0.f)).xyz;

	vec3 lighting = evaluate_ambient_cube(position_vs, normal_ws);

	vec3 diffuse = (1.f - metallic) * color;

	pixel_out.color = cavity * (diffuse * lighting);
}