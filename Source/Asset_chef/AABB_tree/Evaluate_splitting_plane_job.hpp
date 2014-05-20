#pragma once

#include "Triangle.hpp"
#include <vector>

struct Plane;

class Evaluate_splitting_plane_job
{

public:


	Evaluate_splitting_plane_job();
	~Evaluate_splitting_plane_job();

	size_t evaluate(const Plane& splitting_plane, const std::vector<Triangle>& triangles);
};
