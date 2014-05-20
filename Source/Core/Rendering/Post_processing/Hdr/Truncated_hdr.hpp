#pragma once

#include "Hdr_filter.hpp"

namespace rendering
{

class Effect;
class Input_layout;
class Depth_stencil_state;
class Blend_state;

class Truncated_hdr : public Hdr_filter
{

public:

	Truncated_hdr(Rendering_tool& rendering_tool, Data_format::Value hdr_format);

	virtual bool init(Resource_manager& resource_manager);

	virtual void render(const Handle<Shader_resource_view>& source, const Viewport& source_viewport, const Rendering_context& context);

private:

	virtual bool on_resize_targets(const uint2& size);

	bool create_render_states();

	Handle<Effect> effect_;

	Handle<Input_layout> input_layout_;

	Handle<Depth_stencil_state> ds_state_;
	Handle<Blend_state>         blend_state_;
};

}

