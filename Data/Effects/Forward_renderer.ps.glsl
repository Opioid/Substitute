out vec4 outputColor; 

mat3 cotangent_frame(vec3 N, vec3 p, vec2 uv)
{
    // get edge vectors of the pixel triangle
    vec3 dp1 = dFdx(p);
    vec3 dp2 = dFdy(p);
    vec2 duv1 = dFdx(uv);
    vec2 duv2 = dFdy(uv);
 
    // solve the linear system
 //   vec3 dp2perp = cross(dp2, N);
 //   vec3 dp1perp = cross(N, dp1);

    vec3 dp2perp = cross(N, dp2);
    vec3 dp1perp = cross(dp1, N);

    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
 
    // construct a scale-invariant frame 
    float invmax = inversesqrt(max(dot(T, T), dot(B, B)));
    return mat3(-T * invmax, -B * invmax, N);
}

void main() 
{
	vec3 tex_coord = vec3(pixel_in.tex_coord, 0.f);

	vec3 normal = normalize(pixel_in.normal_vs);

#ifdef NORMAL_MAP

	vec3 eye_dir = normalize(pixel_in.vertex_to_eye_vs);

//	eye_dir.z *= 1.f;

    mat3 TBN = cotangent_frame(normal, eye_dir, pixel_in.tex_coord);

/*
#ifdef PARALLAX

    vec3 eye_ts = eye_dir * TBN;
	
	for (uint i = 0; i < 2; ++i)
	{
		vec3 nm = (2.f * (texture(g_normal_map, tex_coord.xy).xyz) - 0.5f);

		float height = texture(g_height_map, tex_coord.xy).x * 0.02f - 0.02f;
		
		tex_coord += (height - tex_coord.z) * nm.z * eye_ts;
	}	

#endif
*/
	vec3 nm = (2.f * (texture(g_normal_map, tex_coord.xy).xyz) - 0.5f);

	normal = normalize(TBN * nm);

#endif

	vec3 diffuse_color = texture(g_diffuse_map, tex_coord.xy).rgb;

	float n_dot_l = max(dot(normal, g_light_direction_vs), 0.00001f);

	outputColor = vec4(n_dot_l * diffuse_color * g_light_energy, 1.f); 

//	outputColor = vec4(0.00001f * n_dot_l * g_light_energy + diffuse_color, 1.f); 

//	normal = (1.f + normal) * 0.5f;

//	outputColor = vec4(0.0001f * (n_dot_l + diffuse_color + g_light_energy) + normal, 1.f); 
}