float soft_fade(float scene_depth, float particle_depth)
{
	return saturate((scene_depth - particle_depth) * 0.9f);
}

/*
float soft_fade(float scene_depth, float particle_depth)
{
	float d = scene_depth - particle_depth;
	
	float fade = 0.5f * pow(saturate(2.f * ((d > 0.5) ? 1.f - d : d)), 0.4f);
	return (d > 0.5) ? 1.f - fade : fade;	
}
*/
void main()
{
	vec4 color = texture(g_color_map, pixel_in.tex_coord_and_factor.xy);

#ifdef SOFT
	ivec2 pixel_coord = ivec2(gl_FragCoord.x, gl_FragCoord.y);

	float depth = texelFetch(g_depth_map, pixel_coord, 0).r;

	float linear_depth = calculate_linear_depth(depth, vec2(g_view_rays[0].w, g_view_rays[1].w));

	float fade = soft_fade(linear_depth, pixel_in.depth);

	color.a *= fade;
#endif // SOFT

	pixel_out.color = pixel_in.tex_coord_and_factor.z * color;
}