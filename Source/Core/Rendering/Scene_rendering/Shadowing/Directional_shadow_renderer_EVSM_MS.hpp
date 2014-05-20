#pragma once

#include "Directional_shadow_renderer.hpp"
#include "Shadow_renderer_EVSM_MS.hpp"

namespace rendering
{

class Directional_shadow_renderer_EVSM_MS : public Directional_shadow_renderer, Shadow_renderer_EVSM_MS
{

public:

	Directional_shadow_renderer_EVSM_MS(Rendering_tool& rendering_tool, Surface_collector& surface_collector, const uint2& size);

	virtual bool init(Resource_manager& resource_manager, Constant_buffer_cache& constant_buffer_cache);

	virtual Handle<Shader_resource_view>& get_shadow_map();

	virtual Handle<Shader_resource_view>& get_white_buffer();

private:

	virtual void prepare_rendering(const Rendering_context& context);

	virtual void render_cascade(const Rendering_context& context, const Cascade_data& cascade_data);

	void apply_shadow(const Rendering_context& context, const Cascade_data& cascade_data);

	virtual bool on_resize_targets(const uint2& dimensions, const Handle<Depth_stencil_shader_resource_view>& depth_stencil);

	bool create_render_states();

	Handle<Render_target_shader_resource_view> white_buffer_;

	struct
	{
		Handle<Framebuffer> framebuffer;

		Handle<Effect> effect;

		Handle<Input_layout> input_layout;
		Handle<Input_layout> volume_input_layout;

		Effect_technique* technique;
		Effect_technique* volume_technique;

		Handle<Rasterizer_state> rasterizer_state;
		Handle<Rasterizer_state> rasterizer_state_upside_down;

		Handle<Depth_stencil_state> ds_state;
		Handle<Depth_stencil_state> volume_ds_state;

		Handle<Blend_state> blend_state;

		struct Filter_kernel
		{
			float4 filter_kernel[4];
		};

		Constant_buffer_updater<Filter_kernel> filter_kernel;

		struct Change_per_cascade_apply
		{
			float4x4 world;
			float4x4 shadow_transform;
			float2   depth_clamp;
		};

		Constant_buffer_updater<Change_per_cascade_apply> change_per_cascade;
	}
	apply_shadow_;
};

}
