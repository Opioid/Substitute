#pragma once

#include "Rendering/Effect/Constant_buffer_updater.hpp"
#include "Math/Matrix.hpp"

namespace scene
{
	class Material;
}

namespace rendering
{

class Rendering_tool;
class Render_state_cache;
class Constant_buffer_cache;
class Input_layout;
struct Viewport;
class Framebuffer;
class Depth_stencil_shader_resource_view;
class Render_target_shader_resource_view;
class Rasterizer_state;
class Depth_stencil_state;
class Blend_state;
struct Render_surface;

class Shadow_renderer_EVSM_MS
{

protected:

	bool init(const uint2& dimensions, Rendering_tool& rendering_tool, Resource_manager& resource_manager, Constant_buffer_cache& constant_buffer_cache);

	void generate_shadow(const float4x4& view_projection, const std::vector<Render_surface>& surfaces, const Viewport& viewport, Rendering_device& device);

	bool prepare_material(const scene::Material* material, Rendering_device& device);

	void resolve_shadow_linear(const Handle<Framebuffer> target, const Handle<Shader_resource_view>& source, const float2& depth_clamp, const Viewport& viewport, Rendering_tool rendering_tool);
	void resolve_shadow_not_linear(const Handle<Framebuffer> target, const Handle<Shader_resource_view>& source, const float2& linear_depth_projection, const Viewport& viewport, Rendering_tool rendering_tool);

	bool create_render_states(Render_state_cache& cache);

	Handle<Depth_stencil_shader_resource_view> depth_;

	Handle<Render_target_shader_resource_view> shadow_map_;

	const scene::Material*  previous_material_;
	const Effect_technique* previous_technique_;
	bool previous_rasterizer_state_;

	// Usually we would want the next two structs to be unnamed, but MSVC linker complained about duplicate Change_per_view

	struct Generate_shadow
	{
		Handle<Framebuffer> framebuffer;

		Handle<Effect> effect;

		Handle<Input_layout> input_layout;
		Handle<Input_layout> alpha_test_input_layout;

		Effect_technique* technique;
		Effect_technique* alpha_test_technique;

		Handle<Depth_stencil_state> ds_state;

		Handle<Blend_state> blend_state;

		struct Change_per_view
		{
			float4x4 view_projection;
		};

		Constant_buffer_updater<Change_per_view> change_per_view;

		struct Change_per_object
		{
			float4x4 world;
		};

		Constant_buffer_updater<Change_per_object> change_per_object;
	};
	Generate_shadow generate_shadow_;

	struct Resolve_shadow
	{
		Handle<Framebuffer> framebuffer;

		Handle<Effect> effect;

		Handle<Input_layout> input_layout;

		Effect_technique* technique_linear;
		Effect_technique* technique_not_linear;

		Handle<Rasterizer_state> rasterizer_state;

		Handle<Depth_stencil_state> ds_state;

		Handle<Blend_state> blend_state;

		struct Change_per_view
		{
			float2 depth_clamp;
		};

		Constant_buffer_updater<Change_per_view> change_per_view;
	};
	Resolve_shadow resolve_shadow_;

	Handle<Rasterizer_state> rasterizer_state_cull_back_;
	Handle<Rasterizer_state> rasterizer_state_cull_none_;

};

}
