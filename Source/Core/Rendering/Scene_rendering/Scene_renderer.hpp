#pragma once

#include "Rendering/Rendering_object.hpp"
#include "Resources/Handle.hpp"
#include "Math/Vector.hpp"

class Resource_manager;

namespace rendering
{

class Depth_stencil_shader_resource_view;
class Constant_buffer_cache;

class Scene_renderer : public Rendering_object
{

public:

	Scene_renderer(Rendering_tool& rendering_tool);
	virtual ~Scene_renderer();

	virtual bool init(Resource_manager& resource_manager, Constant_buffer_cache& constant_buffer_cache) = 0;

	const uint2& target_dimensions() const;

	const Handle<Depth_stencil_shader_resource_view>& depth_stencil() const;

	bool resize_targets(const uint2& dimensions, const Handle<Depth_stencil_shader_resource_view>& depth_stencil);

private:

	virtual bool on_resize_targets(const uint2& dimensions, const Handle<Depth_stencil_shader_resource_view>& depth_stencil) = 0;

	uint2 target_dimensions_;

	Handle<Depth_stencil_shader_resource_view> depth_stencil_;
};

}


