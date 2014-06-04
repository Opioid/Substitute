#ifdef LINEAR_DEPTH

vec3 get_position_vs(vec3 view_ray_vs, float linear_depth)
{
#ifdef VOLUME
	view_ray_vs /= view_ray_vs.z;
#endif

	return linear_depth * view_ray_vs;
}

#else

vec3 get_position_vs(vec3 view_ray_vs, float depth, vec2 linear_depth_projection)
{
#ifdef VOLUME
	view_ray_vs /= view_ray_vs.z;
#endif

	float linear_depth = linear_depth_projection.y / (depth - linear_depth_projection.x);
	
	return linear_depth * view_ray_vs;
}

float calculate_linear_depth(float depth, vec2 linear_depth_projection)
{
	return linear_depth_projection.y / (depth - linear_depth_projection.x);
}

#endif