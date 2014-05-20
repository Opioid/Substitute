#pragma once

#include "Rendering/Post_processing/Post_processing_effect.hpp"
#include "Rendering/Effect/Constant_buffer_updater.hpp"

namespace rendering
{

class Effect;
class Input_layout;
class Depth_stencil_state;
class Blend_state;

class Fxaa : public Post_processing_effect
{

public:

	Fxaa(Rendering_tool& rendering_tool);

	virtual bool init(Resource_manager& resource_manager);

	virtual void render(const Handle<Shader_resource_view>& source, const Viewport& source_viewport, const Rendering_context& context);

private:

	virtual bool on_resize_targets(const uint2& size);

	bool create_render_states();

	Handle<Effect> effect_;

	Handle<Input_layout> input_layout_;

	struct Change_per_source
	{
		float2 inverse_source_size;
	};

	Constant_buffer_updater<Change_per_source> change_per_source_;

	Handle<Depth_stencil_state> ds_state_;
	Handle<Blend_state>         blend_state_;
};

}


