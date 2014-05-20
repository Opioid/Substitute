float calculate_shadow_occlusion(vec3 position_vs)
{
	vec4 shadow_projection = (g_shadow_transform * vec4(position_vs, 1.f));

//	float clamped_depth = (shadow_projection.z - g_depth_clamp.x) * g_depth_clamp.y - 0.001f;

	float depth = 0.5f * (shadow_projection.z + 1.f);

	float clamped_depth = g_depth_clamp.y * (depth - g_depth_clamp.x) - 0.001f;

	vec2 warped_depth = warp_depth(clamped_depth, vec2(40.f, 40.f));

	vec4 moments = vec4(0.f, 0.f, 0.f, 0.f);

	for (uint i = 0; i < 4; ++i)
	{
		moments += g_filter_kernel[i].z * texture(g_shadow_map, shadow_projection.xy + g_filter_kernel[i].xy); 
	}

	float a = chebyshev_upper_bound(moments.xz, warped_depth.x);
	float b = chebyshev_upper_bound(moments.yw, warped_depth.y);
	
	return min(a, b);
}

void main() 
{
	ivec2 pixel_coord = ivec2(gl_FragCoord.x, gl_FragCoord.y);

	float depth = texelFetch(g_depth_map, pixel_coord, 0).r;
	
	vec3 position_vs = get_position_vs(pixel_in.view_ray, depth, vec2(g_view_rays[0].w, g_view_rays[1].w));

	pixel_out.shadow = calculate_shadow_occlusion(position_vs); 
}