#include "Deferred_shadow_renderer.hpp"
#include "Rendering/Resource_view.hpp"
#include "Scene/Scene.hpp"
#include "Math/Intersection/Polyhedron.hpp"

namespace rendering
{

Deferred_shadow_renderer::Deferred_shadow_renderer(Rendering_tool &rendering_tool, Surface_collector& surface_collector, const uint2& size) :
	Shadow_renderer(rendering_tool, surface_collector, size)
{}

void Deferred_shadow_renderer::set_deferred_depth(const Handle<Shader_resource_view>& depth)
{
	deferred_depth_ = depth;
}

}
