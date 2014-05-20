#include "Math.hpp"

namespace math
{

const float pi = 3.14159265f;
const float pi_mul_2 = pi * 2.f;
const float pi_div_2 = pi * 0.5f;
const float pi_div_180 = pi / 180.f;
const float _180_div_pi = 180.f / pi;

float3 random_sphere()
{
	float z = random(-1.f, 1.f);
	float a = random(0.f, math::pi_mul_2);
	float r = sqrt(1.f - z * z);
	float x = r * cos(a);
	float y = r * sin(a);

	return float3(x, y, z);
}

}
