void main() 
{
	ivec2 pixel_coord = ivec2(gl_FragCoord.x, gl_FragCoord.y);

	float depth  = texelFetch(g_depth_map, pixel_coord, 0).r;
	vec3 normal  = decode_normal(texelFetch(g_normal_map, pixel_coord, 0).xy);
	vec3 color   = texelFetch(g_color_map, pixel_coord, 0).rgb;
	vec3 surface = texelFetch(g_surface_map, pixel_coord, 0).xyz;

	float metallic = surface.x;
	float roughness = surface.y;
	float cavity = surface.z;
	
	vec3 position_vs = get_position_vs(pixel_in.view_ray, depth, vec2(g_view_rays[0].w, g_view_rays[1].w));

	vec3 view_direction = normalize(-position_vs);	// eyePos is (0, 0, 0) in viewSpace

#ifdef VOLUME

	vec3 ray_vs = reflect(position_vs, normal);	

	vec3 ray_ls = (g_light_data  * vec4(ray_vs, 0.f)).xyz;
	vec3 position_ls = (g_light_data * vec4(position_vs, 1.f)).xyz;
	
	vec3 unitary = vec3(1.f, 1.f, 1.f);
	vec3 first_plane_intersect  = (unitary - position_ls) / ray_ls;
	vec3 second_plane_intersect = (-unitary - position_ls) / ray_ls;
	vec3 furthest_plane = max(first_plane_intersect, second_plane_intersect);
	float dist = min(furthest_plane.x, min(furthest_plane.y, furthest_plane.z));

	
//	float3 position_ws = mul(float4(position_vs, 1.f), g_view_inv);
//	float3 ray_ws = mul(float4(ray_vs, 0.f), g_view_inv);
	
	// Use Distance in WS directly to recover intersection
//	float3 intersection_ws = position_ws + distance * ray_ws;
//	float3 direction_ws = intersection_ws - float3(-1, 1, -3);//CubemapPositionWS;
	
	
	//This is supposed to happen in world space, but the multiplication with distance seems to look the same in view space and is more convenient in our pipeline
	vec3 intersection_vs = position_vs + dist * ray_vs;
	vec3 reflection_ws = (g_inverse_view * vec4(g_light_position_vs - intersection_vs, 0.f)).xyz;

#else

	vec3 reflection_vs = reflect(view_direction, normal);

	vec3 reflection_ws = (g_inverse_view * vec4(reflection_vs, 0.f)).xyz;

#endif // VOLUME

	vec3 f0 = lerp(vec3(0.04f, 0.04f, 0.04f), color, metallic);

	vec2 brdf = texture(g_light_2D_map0, vec2(roughness, dot(view_direction, normal))).rg;

	// the explicit lod is roughness * number of mip levels in the pre-filtered environment map
	vec3 prefiltered_environment = textureLod(g_light_probe_map, reflection_ws, roughness * 9.f).rgb;

	pixel_out.color = cavity * prefiltered_environment * (f0 * brdf.x + brdf.y);
}