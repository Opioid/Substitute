vec3 lighting(vec3 position, vec3 normal, vec3 light_direction, vec3 color, float metallic, float roughness, float cavity);
float light_falloff(float radius, vec3 v);

void main() 
{
	ivec2 pixel_coord = ivec2(gl_FragCoord.x, gl_FragCoord.y);

	float depth  = texelFetch(g_depth_map, pixel_coord, 0).r;
	vec3 normal  = decode_normal(texelFetch(g_normal_map, pixel_coord, 0).xy);
	vec3 color   = texelFetch(g_color_map, pixel_coord, 0).rgb;
	vec3 surface = texelFetch(g_surface_map, pixel_coord, 0).xyz;
	
	vec3 position_vs = get_position_vs(pixel_in.view_ray, depth, vec2(g_view_rays[0].w, g_view_rays[1].w));

	vec3 light_energy = g_light_energy_and_range.xyz;

#ifdef DIRECTIONAL

	vec3 light_direction_vs = g_light_direction_vs;

	#ifdef SHADOW

		// We do deferred shadowing for directional lights, so only fetch the shadow mask here
		light_energy *= texelFetch(g_light_2D_map1, pixel_coord, 0).r;

	#endif // SHADOW

#elif defined POINT || defined SPOT

	vec3 light_vector = g_light_position_vs - position_vs;
	vec3 light_direction_vs = normalize(light_vector);

	float light_range = g_light_energy_and_range.w;

	light_energy *= light_falloff(light_range, light_vector);

	#ifdef SPOT

		vec4 position_ls = g_light_transformation * vec4(position_vs, 1.f);

		vec3 light_projection = position_ls.xyz / position_ls.w;

		vec3 mask = textureLod(g_light_2D_map0, light_projection.xy, 0.f).rgb; 

		light_energy *= mask;

		#ifdef SHADOW

			float depth_ls = 0.5f * (light_projection.z + 1.f);

			light_energy *= calculate_spot_shadow_occlusion(depth_ls, light_projection.xy);

		#endif // SHADOW

	#endif // SPOT

#endif // DIRECTIONAL

	vec3 light = lighting(position_vs, normal, light_direction_vs, color, surface.x, surface.y, surface.z);

	pixel_out.color = light * light_energy; 
}

// Lighting model based on Unreal Engine 4 style GGX
// http://graphicrants.blogspot.de/2013/08/specular-brdf-reference.html
// http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf

#define pi 3.14159265f

vec3 specular_f(float v_dot_h, vec3 f0)
{
	return f0 + (1.f - f0) * exp2((-5.55473 * v_dot_h - 6.98316) * v_dot_h);
}

// GGX/Trowbridge-Reitz
float specular_d(float n_dot_h, float a2)
{
	float d = n_dot_h * n_dot_h * (a2 - 1.f) + 1.f;
	return a2 / (pi * d * d);
}

float specular_g(float n_dot_l, float n_dot_v, float a2)
{
	float G_V = n_dot_v + sqrt((n_dot_v - n_dot_v * a2) * n_dot_v + a2);
	float G_L = n_dot_l + sqrt((n_dot_l - n_dot_l * a2) * n_dot_l + a2);
	return inversesqrt(G_V * G_L);
}

vec3 lighting(vec3 position, vec3 normal, vec3 light_direction, vec3 color, float metallic, float roughness, float cavity)
{
	vec3 view_direction = normalize(-position);	// eyePos is (0, 0, 0) in view_space

	vec3 half_vector = normalize(view_direction + light_direction);

	float n_dot_l = max(dot(normal, light_direction), 0.00001f);
	float n_dot_v = max(dot(normal, view_direction),  0.00001f);

	float n_dot_h = dot(normal, half_vector);
	float v_dot_h = dot(view_direction, half_vector);

	vec3 f0 = lerp(vec3(0.03f, 0.03f, 0.03f), color, metallic);

	float a = roughness * roughness;
	float a2 = a * a;

	vec3 specular = specular_f(v_dot_h, f0) * specular_d(n_dot_h, a2) * specular_g(n_dot_l, n_dot_v, a2);

	vec3 diffuse = (1.f - metallic) * color;
/*
	if (isnan(normal.x) || isnan(normal.y) || isnan(normal.z))
	{
		return vec3(1.f, 0.f, 0.f);
	}
*/
	return cavity * n_dot_l * (diffuse + specular);
}

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