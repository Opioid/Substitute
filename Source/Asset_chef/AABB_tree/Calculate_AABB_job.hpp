#pragma once

//#include "Threading/Job.hpp"
#include "Triangle.hpp"
#include "Math/Intersection/AABB.hpp"
#include <vector>

//namespace threading { class Worker_pool; }

class Calculate_AABB_job //: public threading::Job
{

public:

	AABB calculate_AABB(const std::vector<Triangle>& triangles);

private:

};
