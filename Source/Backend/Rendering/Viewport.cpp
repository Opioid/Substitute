#include "Viewport.hpp"

namespace rendering
{

Viewport::Viewport() : origin(0.f, 0.f), size(0.f, 0.f), depth_range(0.f, 1.f)
{}

Viewport::Viewport(const float2& origin, const float2& size) : origin(origin), size(size), depth_range(0.f, 1.f)
{}

Viewport::Viewport(const float2& origin, const float2& size, const float2& depth_range) : origin(origin), size(size), depth_range(depth_range)
{}

}
