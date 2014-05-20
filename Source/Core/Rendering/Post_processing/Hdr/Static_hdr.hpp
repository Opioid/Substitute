#pragma once

#include "Hdr_filter.hpp"
#include "Rendering/Effect/Constant_buffer_updater.hpp"

namespace rendering
{

class Effect;
class Effect_technique;
class Input_layout;
class Depth_stencil_state;
class Blend_state;

class Static_hdr : public Hdr_filter
{

public:

	Static_hdr(Rendering_tool& rendering_tool, Data_format::Value hdr_format);

	virtual bool init(Resource_manager& resource_manager);

	virtual void render(const Handle<Shader_resource_view>& source, const Viewport& source_viewport, const Rendering_context& context);

private:

	virtual bool on_resize_targets(const uint2& dimensions);

	bool create_render_states();

	Handle<Effect> effect_;

	Effect_technique* compose_technique_;

	Handle<Input_layout> input_layout_;

	Handle<Depth_stencil_state> ds_state_;

	Handle<Blend_state> compose_blend_state_;

	struct Change_per_setting
	{
		float4 linear_white_and_exposure;
	};

	Constant_buffer_updater<Change_per_setting> change_per_setting_;
};

}
