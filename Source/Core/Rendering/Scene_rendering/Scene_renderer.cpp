#include "Scene_renderer.hpp"
#include "Rendering/Resource_view.hpp"

namespace rendering
{

Scene_renderer::Scene_renderer(Rendering_tool& rendering_tool) : Rendering_object(rendering_tool), target_dimensions_(0, 0)
{}

Scene_renderer::~Scene_renderer()
{}

const uint2& Scene_renderer::target_dimensions() const
{
	return target_dimensions_;
}

const Handle<Depth_stencil_shader_resource_view>& Scene_renderer::get_depth_stencil() const
{
	return depth_stencil_;
}

bool Scene_renderer::resize_targets(const uint2& dimensions, const Handle<Depth_stencil_shader_resource_view>& depth_stencil)
{
	if (target_dimensions_ == dimensions)
	{
		return true;
	}

	bool result = on_resize_targets(dimensions, depth_stencil);

	if (result)
	{
		target_dimensions_ = dimensions;
		depth_stencil_ = depth_stencil;
	}
	else
	{
		target_dimensions_ = uint2(0, 0);
		depth_stencil_ = nullptr;
	}

	return result;
}

}
