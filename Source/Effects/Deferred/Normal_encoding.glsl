// Spheremap transform; only suitable for view space normals, but pretty good quality
// http://aras-p.info/texts/CompactNormalStorage.html

/*
vec2 encode_normal_spheremap(vec3 normal)
{
    vec2 enc = normalize(normal.xy) * (sqrt(-normal.z * 0.5f + 0.5f));
    enc = enc * 0.5f + 0.5f;
    return enc;
}

vec3 decode_normal_spheremap(vec2 enc)
{
    vec4 nn = enc.xyxy * vec4(2.f, 2.f, 0.f, 0.f) + vec4(-1.f, -1.f, 1.f, -1.f);
    float l = dot(nn.xyz, -nn.xyw);
    nn.z = l;
    nn.xy *= sqrt(l);
    return nn.xyz * 2.f + vec3(0, 0, -1.f);
}
*/


#define SPHEREMAP
// #define OCTAHEDRON

#ifdef SPHEREMAP

// encode_normal_spheremap
vec2 encode_normal(vec3 normal)
{
	float p = sqrt(-normal.z * 8.f + 8.f);
	return normal.xy / p + 0.5f;
//	return min(normal.xy / p + 0.5f, 0.853f); // min to avoid nan in decoding, probably due to bad normals?!
}


// decode_normal_spheremap
vec3 decode_normal(vec2 enc)
{
	vec2 fenc = enc * 4.f - 2.f;
	float f = dot(fenc, fenc);
	float g = sqrt(1.f - f * 0.25f);

	vec3 normal;
	normal.xy = fenc * g;
	normal.z = -(1.f - f * 0.5f);
	return normal;	
}

#elif defined OCTAHEDRON

// Returns ±1
vec2 sign_not_zero(vec2 v) 
{
	return vec2((v.x >= 0.f) ? +1.f : -1.f, (v.y >= 0.f) ? + 1.f : -1.f);
}

// float32x3_to_oct
// Assume normalized input. Output is on [-1, 1] for each component.
vec2 encode_normal(vec3 normal) 
{
	// Project the sphere onto the octahedron, and then onto the xy plane
	vec2 p = normal.xy * (1.f / (abs(normal.x) + abs(normal.y) + abs(normal.z)));
	// Reflect the folds of the lower hemisphere over the diagonals
	return (normal.z <= 0.f) ? ((1.f - abs(p.yx)) * sign_not_zero(p)) : p;
}

// oct_to_float32x3
vec3 decode_normal(vec2 enc) 
{
	vec3 v = vec3(enc.xy, 1.f - abs(enc.x) - abs(enc.y));

	if (v.z < 0.f)
	{
		v.xy = (1.f - abs(v.yx)) * sign_not_zero(v.xy);
	}

	return normalize(v);
}

#endif