#pragma once

#include "Triangle.hpp"
#include <vector>

struct Plane;

namespace scene { class AABB_node; }

class Clipping_job
{

public:

  Clipping_job();
  ~Clipping_job();

  void clip(std::vector<Triangle>& result, const std::vector<Triangle>& source, const Plane& splitting_plane);

private:

  std::vector<Triangle> result_;
};
