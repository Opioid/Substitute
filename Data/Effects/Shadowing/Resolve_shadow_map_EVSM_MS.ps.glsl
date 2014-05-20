void main()
{
	ivec2 pixel_coord = ivec2(gl_FragCoord.x, gl_FragCoord.y);

	vec4 moments = vec4(0.f, 0.f, 0.f, 0.f);

	for (int i = 0; i < 4; ++i)
	{
		float depth = texelFetch(g_ms_source, pixel_coord, i).r;

#ifdef LINEAR

		float clamped_depth = g_depth_clamp.y * max(depth - g_depth_clamp.x, 0.f);

		moments += generate_evsm_moments(clamped_depth);

#else

		float linear_depth = g_depth_clamp.y / (depth - g_depth_clamp.x);

		moments += generate_evsm_moments(linear_depth);

#endif // LINEAR

	}

	pixel_out.evsm_moments = 0.25f * moments;
}