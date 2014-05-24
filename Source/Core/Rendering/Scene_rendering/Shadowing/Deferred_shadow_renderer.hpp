#pragma once

#include "Shadow_renderer.hpp"

namespace rendering
{

class Deferred_shadow_renderer : public Shadow_renderer
{

public:

	Deferred_shadow_renderer(Rendering_tool& rendering_tool, Surface_collector& surface_collector, const uint2& size);

    void set_deferred_depth(const Handle<Shader_resource_view>& depth);

	virtual Handle<Shader_resource_view>& white_buffer() = 0;

protected:

	Handle<Shader_resource_view> deferred_depth_;
};

}
