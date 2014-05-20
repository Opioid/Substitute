#include "Evaluate_splitting_plane_job.hpp"
#include "Clipping.hpp"
#include "Math/Plane.hpp"


Evaluate_splitting_plane_job::Evaluate_splitting_plane_job()
{}

Evaluate_splitting_plane_job::~Evaluate_splitting_plane_job()
{}

size_t Evaluate_splitting_plane_job::evaluate(const Plane& splitting_plane, const std::vector<Triangle>& triangles)
{
	size_t clips = 0;

	for (size_t i = 0; i < triangles.size(); ++i)
	{
		if (triangle_intersect_plane(triangles[i], splitting_plane))
		{
			++clips;
		}
	}

	return clips;
}

