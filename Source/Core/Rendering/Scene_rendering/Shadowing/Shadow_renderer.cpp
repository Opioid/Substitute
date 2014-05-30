#include "Shadow_renderer.hpp"
#include "Math/Vector.inl"

namespace rendering
{

Shadow_renderer::Shadow_renderer(Rendering_tool& rendering_tool, Surface_collector& surface_collector, const uint2& dimensions) :
	Scene_renderer(rendering_tool), surface_collector_(surface_collector), shadow_map_dimensions_(dimensions), viewport_(float2(0.f, 0.f), float2(dimensions))
{}

}
