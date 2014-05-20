float calculate_spot_shadow_occlusion(float depth, vec2 shadow_projection)
{
//	float clamped_depth = (g_shadow_depth_clamp.x + depth) * g_shadow_depth_clamp.y - 0.001f;

	float linear_depth = g_shadow_linear_depth_projection.y / (depth - g_shadow_linear_depth_projection.x) - 0.001f;

	vec2 warped_depth = warp_depth(linear_depth, vec2(40.f, 40.f));

	vec4 moments = vec4(0.f, 0.f, 0.f, 0.f);

	for (uint i = 0; i < 4; ++i)
	{
		moments += g_filter_kernel[i].z * texture(g_light_2D_map1, shadow_projection.xy + g_filter_kernel[i].xy); 
	}

	float a = chebyshev_upper_bound(moments.xz, warped_depth.x);
	float b = chebyshev_upper_bound(moments.yw, warped_depth.y);
	
	return min(a, b);
}