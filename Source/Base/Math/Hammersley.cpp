#include "Hammersley.hpp"
#include "Math/Vector.inl"

namespace math
{

float radical_inverse_VdC(uint32_t bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xaaaaaaaau) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xccccccccu) >> 2u);
	bits = ((bits & 0x0f0f0f0fu) << 4u) | ((bits & 0xf0f0f0f0u) >> 4u);
	bits = ((bits & 0x00ff00ffu) << 8u) | ((bits & 0xff00ff00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10f; // / 0x100000000
}

float2 hammersley(uint32_t i, uint32_t num_samples)
{
	return float2(float(i) / float(num_samples), radical_inverse_VdC(i));
}

}
