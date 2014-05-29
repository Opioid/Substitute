vec3 unpack_BC5_normal(vec4 n)
{
	vec2 xy = 2.f * (n.gr - 0.5f);
	float  z  = sqrt(1.f - dot(xy, xy));
	return vec3(xy, z);
}

mat3 cotangent_frame(vec3 N, vec3 p, vec2 uv)
{
    // get edge vectors of the pixel triangle
    vec3 dp1 = dFdx(p);
    vec3 dp2 = dFdy(p);
    vec2 duv1 = dFdx(uv);
    vec2 duv2 = dFdy(uv);
 
    // solve the linear system
    vec3 dp2perp = cross(dp2, N);
    vec3 dp1perp = cross(N, dp1);

    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
 
    // construct a scale-invariant frame 
    float invmax = inversesqrt(max(dot(T, T), dot(B, B)));
    return mat3(T * invmax, B * invmax, N);
}

void main() 
{
	vec3 tex_coord = vec3(pixel_in.tex_coord, 0.f);

	vec3 normal = normalize(pixel_in.normal_vs);

#ifdef NORMAL_MAP

    mat3 TBN = cotangent_frame(normal, pixel_in.vertex_to_eye_vs, pixel_in.tex_coord);

	#ifdef PARALLAX

	vec2 height_scale = g_metallic_and_roughness_and_height_scale.zw;

	vec3 eye_dir = normalize(pixel_in.vertex_to_eye_vs);

    vec3 eye_ts = eye_dir * TBN;
	
	for (uint i = 0; i < 2; ++i)
	{
		vec3 nm = unpack_BC5_normal(texture(g_normal_map, tex_coord.xy));

		float height = texture(g_surface1_map, tex_coord.xy).x * height_scale.x + height_scale.y;
		
		tex_coord += (height - tex_coord.z) * nm.z * eye_ts;
	}	

	#endif // PARALLAX
#endif // NORMAL_MAP

#ifdef COLOR_MAP

	vec4 color = texture(g_color_map, tex_coord.xy);

	#ifdef ALPHA_TEST

	if (color.a < 0.4f)
	{
		discard;
	}

	#endif // ALPHA_TEST

	#ifdef EMISSIVE_MAP

	float emissive_factor = texture(g_surface1_map, tex_coord.xy).y;

	#else

	float emissive_factor = g_color_and_emissive_factor.w;

	#endif // EMISSIVE_MAP

	vec4 color_and_emissive_factor = vec4(color.rgb, emissive_factor);

#else

	vec4 color_and_emissive_factor = g_color_and_emissive_factor;

#endif // COLOR_MAP

#ifdef NORMAL_MAP

	vec3 nm = unpack_BC5_normal(texture(g_normal_map, tex_coord.xy));
	normal = normalize(TBN * nm);

#endif // NORMAL_MAP

	if (!gl_FrontFacing)
	{
		normal *= -1.f;
	}

#ifdef SURFACE_MAP

	vec3 surface = texture(g_surface0_map, tex_coord.xy).xyz;

#else

	vec3 surface = vec3(g_metallic_and_roughness_and_height_scale.xy, 1.f);

#endif // SURFACE_MAP

	pixel_out.normal  = encode_normal(normal);
	pixel_out.color   = color_and_emissive_factor; 
	pixel_out.surface = surface;
}