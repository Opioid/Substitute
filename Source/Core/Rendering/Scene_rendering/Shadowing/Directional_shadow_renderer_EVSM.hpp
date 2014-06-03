#pragma once

#include "Directional_shadow_renderer.hpp"
#include "Rendering/Effect/Constant_buffer_updater.hpp"
#include "Math/Matrix.hpp"

namespace scene
{
	class Material;
}

namespace rendering
{

class Directional_shadow_renderer_EVSM : public Directional_shadow_renderer
{

public:

	Directional_shadow_renderer_EVSM(Rendering_tool& rendering_tool, Surface_collector& surface_collector, const uint2& size);

	virtual bool init(Resource_manager& resource_manager, Constant_buffer_cache& constant_buffer_cache);

	virtual Handle<Shader_resource_view>& shadow_map();

	virtual Handle<Shader_resource_view>& white_buffer();

private:

	virtual void render_cascade(const Rendering_context& context, const Cascade_data& cascade_data);

	void generate_shadow(const Cascade_data& cascade_data);

	bool prepare_material(const scene::Material* material);

	void apply_shadow(const Rendering_context& context, const Cascade_data& cascade_data);

	virtual bool on_resize_targets(const uint2& size, const Handle<Depth_stencil_shader_resource_view>& depth_stencil);

	bool create_render_states();

	Handle<Depth_stencil_view> depth_;

	Handle<Render_target_shader_resource_view> shadow_map_;

	Handle<Render_target_shader_resource_view> white_buffer_;

	const scene::Material*  previous_material_;
	const Effect_technique* previous_technique_;
	bool previous_rasterizer_state_;

	struct
	{
		Handle<Framebuffer> framebuffer;

		Handle<Effect> effect;

		Handle<Input_layout> input_layout;
		Handle<Input_layout> alpha_test_input_layout;

		Effect_technique* technique;
		Effect_technique* alpha_test_technique;

		Handle<Depth_stencil_state> ds_state;

		Handle<Blend_state> blend_state;

		struct Change_per_cascade
		{
			float2 depth_clamp;
		};

		Constant_buffer_updater<Change_per_cascade> change_per_cascade;

		struct Change_per_object
		{
			float4x4 world_view_projection;
		};

		Constant_buffer_updater<Change_per_object> change_per_object;
	}
	generate_shadow_;

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

		struct Change_per_camera
		{
			float4   rays[3];
			float4x4 view_projection;
			float4x4 view;
			float4x4 inverse_view;
		};

		Constant_buffer_updater<Change_per_camera> change_per_camera;

		struct Change_per_cascade_apply
		{
			float4x4 world;
			float4x4 shadow_transform;
			float2   depth_clamp;
		};

		Constant_buffer_updater<Change_per_cascade_apply> change_per_cascade;
	}
	apply_shadow_;

	Handle<Rasterizer_state> rasterizer_state_cull_back_;
	Handle<Rasterizer_state> rasterizer_state_cull_none_;
};

}
