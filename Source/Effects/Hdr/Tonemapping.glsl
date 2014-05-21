const vec3 g_luminance_vector = vec3(0.299f, 0.587f, 0.114f);

const float max_luminance = 32.f; 

float luminance(vec3 color)
{
	return dot(color, g_luminance_vector);
}

// Function used by the Uncharte2D tone mapping curve
vec3 tonemap_function(vec3 color)
{/*
	float A = 0.15f;
	float B = 0.50f;
	float C = 0.10f;
	float D = 0.20f;
	float E = 0.02f;
	float F = 0.30f;
	*/

	float A = 0.22f;
	float B = 0.30f;
	float C = 0.10f;
	float D = 0.20f;
	float E = 0.01f;
	float F = 0.30f;

	return ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
}


// Applies the Uncharted 2 filmic tone mapping curve
vec3 tonemap_unchartered(vec3 color)
{
	vec3 numerator   = tonemap_function(color);        
	vec3 denominator = tonemap_function(g_linear_white_and_exposure.xyz);

	return numerator / denominator;
}

vec3 expose(vec3 color, float average_luminance, float threshold)
{        
	average_luminance = max(average_luminance, 0.001f);

	float key_value = 1.03f - (2.f / (2.f + log10(average_luminance + 1.f)));

	float linear_exposure = (key_value / average_luminance);
	float exposure = log2(max(linear_exposure, 0.0001f));

	exposure -= threshold;

	return exp2(exposure) * color;
}

vec3 expose(vec3 color, float threshold)
{        
	float exposure = g_linear_white_and_exposure.w;

	exposure -= threshold;

	return exp2(exposure) * color;
}

vec3 tonemap(vec3 color, float threshold)
{
	color = expose(color, threshold);

	return tonemap_unchartered(color);
}

vec3 tonemap(vec3 color, float threshold, vec2 texture_coordinates)
{
	/*
	float average_luminance = exp(textureLod(g_luminance_map, texture_coordinates, 10.f).r);

	color = expose(color, average_luminance, threshold);
	*/
	/*
	float pixel_luminance = max(luminance(color), 0.0001f);  
	float tonemapped_luminance = (pixel_luminance * (1.f + (pixel_luminance / (max_luminance * max_luminance)))) / (1.f + pixel_luminance); 
	return tonemapped_luminance * color / pixel_luminance;
	*/
	
//	return tonemap_stevem(color);

	color = expose(color, threshold);

	return tonemap_unchartered(color);
}