#include "Calculate_AABB_job.hpp"
#undef min
#undef max

AABB Calculate_AABB_job::calculate_AABB(const std::vector<Triangle>& triangles)
{
	float3 minv( 100000.0f, 100000.0f, 100000.0f);
	float3 maxv(-100000.0f, -100000.0f, -100000.0f);

	for (size_t i = 0; i < triangles.size(); ++i)
	{
		const Triangle& t = triangles[i];

		for (size_t j = 0; j < 3; ++j)
		{
			minv = math::min(minv, t.v[j].position);
			maxv = math::max(maxv, t.v[j].position);
		}
	}

	return AABB(minv, maxv);
}


