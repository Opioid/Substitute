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

vec2 encode_normal_spheremap(vec3 normal)
{
	float p = sqrt(-normal.z * 8.f + 8.f);
	return normal.xy / p + 0.5f;
//	return min(normal.xy / p + 0.5f, 0.853f); // min to avoid nan in decoding, probably due to bad normals?!
}

vec3 decode_normal_spheremap(vec2 enc)
{
	vec2 fenc = enc * 4.f - 2.f;
	float f = dot(fenc, fenc);
	float g = sqrt(1.f - f * 0.25f);

	vec3 normal;
	normal.xy = fenc * g;
	normal.z = -(1.f - f * 0.5f);
	return normal;	
}