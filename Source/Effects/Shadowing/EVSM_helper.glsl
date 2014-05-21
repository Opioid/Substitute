// Convert depth to EVSM coefficients
// Input depth should be in [0, 1]
vec2 warp_depth(float clamped_depth, vec2 exponents)
{
	// Rescale depth into [-1, 1]
	float depth = 2.f * clamped_depth - 1.f;
	
	float pos =  exp( exponents.x * depth);
	float neg = -exp(-exponents.y * depth);
	
	return vec2(pos, neg);
}

//static const vec2 g_exponents = vec2(40.f, 40.f);

vec4 generate_evsm_moments(float clamped_depth)
{
	vec2 warped_depth = warp_depth(clamped_depth, vec2(40.f, 40.f));
	
	return vec4(warped_depth.xy, warped_depth.xy * warped_depth.xy);
}

float linstep(float min, float max, float v)  
{  
	return clamp((v - min) / (max - min), 0.f, 1.f);
}  

float reduce_light_bleeding(float p_max, float amount)  
{  
	// Remove the [0, Amount] tail and linearly rescale (Amount, 1].
	return linstep(amount, 1, p_max);
//	return smoothstep(amount, 1.f, p_max);
} 

float chebyshev_upper_bound(vec2 moments, float t)  
{  
	//One-tailed inequality valid if t > Moments.x  
	float p = t <= moments.x ? 1.f : 0.f;  
   
	//Compute variance.  
	float variance = moments.y - (moments.x * moments.x);
   
	//Compute probabilistic upper bound.  
	float d = t - moments.x;  
	
	float p_max = variance / (variance + d * d);  
	
	p_max = reduce_light_bleeding(p_max, 0.025f);
	
	return max(p, p_max);  
} 