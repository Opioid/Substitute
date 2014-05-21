

float radical_inverse_VdC(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xaaaaaaaau) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xccccccccu) >> 2u);
	bits = ((bits & 0x0f0f0f0fu) << 4u) | ((bits & 0xf0f0f0f0u) >> 4u);
	bits = ((bits & 0x00ff00ffu) << 8u) | ((bits & 0xff00ff00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10f; // / 0x100000000
}

vec2 hammersley(uint i, uint num_samples)
{
	return vec2(float(i) / float(num_samples), radical_inverse_VdC(i));
}

#define pi 3.14159265f

vec3 importance_sample_GGX(vec2 xi, float roughness, vec3 n)
{
	float a = roughness * roughness;

	float phi = 2.f * pi * xi.x;

	float cos_theta = sqrt((1.f - xi.y) / (1.f + (a * a - 1.f) * xi.y ));

	float sin_theta = sqrt(1.f - cos_theta * cos_theta );

	vec3 h = vec3(sin_theta * cos(phi), sin_theta * sin(phi), cos_theta);

	vec3 up = abs(n.z) < 0.999f ? vec3(0.f, 0, 1.f) : vec3(1.f, 0.f, 0.f);

	vec3 tangent_x = normalize(cross(up, n));

	vec3 tangent_y = cross(n, tangent_x);

	// Tangent to world space
	return h.x * tangent_x + h.y * tangent_y + h.z * n;
}



void main() 
{
	vec3 R = pixel_in.view_ray_ws;

	vec3 N = R;
	vec3 V = R;
	vec3 PrefilteredColor = vec3(0.f, 0.f, 0.f);
	float TotalWeight = 0.f;


	const uint NumSamples = 1024;
	for( uint i = 0; i < NumSamples; i++ )
	{
		vec2 Xi = hammersley( i, NumSamples );
		vec3 H = importance_sample_GGX( Xi, g_roughness, N );
		vec3 L = 2 * dot( V, H ) * H - V;
		float NoL = saturate( dot( N, L ) );
		if (NoL > 0.f)
		{
			PrefilteredColor += texture(g_cubemap, L).rgb * NoL;
			TotalWeight += NoL;
		}
	}

	pixel_out.color = PrefilteredColor / TotalWeight;
}