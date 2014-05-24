#pragma once

#include "Hdr_filter.hpp"
#include "Rendering/Viewport.hpp"
#include "Rendering/Effect/Constant_buffer_updater.hpp"

namespace rendering
{

class Render_target_shader_resource_view;
class Framebuffer;
class Effect;
class Effect_technique;
class Input_layout;
class Depth_stencil_state;
class Blend_state;

class Dynamic_hdr : public Hdr_filter
{

public:

	Dynamic_hdr(Rendering_tool& rendering_tool, Data_format::Value hdr_format);

	virtual bool init(Resource_manager& resource_manager);

	virtual void render(const Handle<Shader_resource_view>& source, const Viewport& source_viewport, const Rendering_context& context);

private:

	virtual bool on_resize_targets(const uint2& dimensions);

	void calculate_average_luminance();

	void bloom();

	void prepare_filter_kernel(const uint2& dimensions);

	bool create_render_states();

	Viewport bloom_viewport_;
	Handle<Render_target_shader_resource_view> bloom_targets_[2];
	Handle<Framebuffer> bloom_framebuffers_[2];

	Viewport luminance_viewport_;
	Handle<Render_target_shader_resource_view> luminance_target_;
	Handle<Framebuffer> luminance_framebuffer_;

	Handle<Effect> effect_;

	Effect_technique* compose_technique_;
	Effect_technique* luminance_technique_;
	Effect_technique* scale_down_technique_;
	Effect_technique* bright_pass_technique_;
	Effect_technique* blur_horizontal_technique_;
	Effect_technique* blur_vertical_technique_;

	Handle<Input_layout> input_layout_;

	Handle<Depth_stencil_state> ds_state_;

	Handle<Blend_state> compose_blend_state_;
	Handle<Blend_state> luminance_blend_state_;
	Handle<Blend_state> bloom_blend_state_;

	struct Change_per_scene
	{
		float3 linear_white;
	};

	Constant_buffer_updater<Change_per_scene> change_per_scene_;

	struct Filter_kernel
	{
		float4 filter_kernel[4];

		float2 texel_size;
	};

	Constant_buffer_updater<Filter_kernel> filter_kernel_;
};

}



/*
#include "HdrFilter.hpp"
#include "Resources/Handle.hpp"
#include "Rendering/Effect.hpp"
#include "Rendering/Render_state.hpp"


class Render_target_shader_resource_view;
class InputLayout;


class Standard_Hdr : public HdrFilter
{
public:

	Standard_Hdr(Render_tool& rendering_tool);
	~Standard_Hdr();

	bool init(const uint2& size, Data_format::Value format, Resource_manager& resource_manager);

	void resolve(const Render_context& context, const Shader_resource_view* source, const float speed);

private:

	void calculate_average_luminance(const Shader_resource_view* source, const uint2& size);

	void bloom(const Shader_resource_view* source, const uint2& size);

	void prepare_down_sample_kernel4x4(const float2& texel_size, bool bilinear = false);

	bool create_render_states();

	uint2 m_size_div4;
	uint2 m_luminance_size;

	float2 m_filter_kernel[16];

	Handle<Effect> effect_;

	Handle<Render_target_shader_resource_view> m_targets[2];

	Handle<Render_target_shader_resource_view> m_luminance;

	const InputLayout* layout_;

	Depth_stencil_state ds_state_;

	Blend_state blend_state_;

	Blend_state blend_state__luminance;

	Blend_state blend_state__bloom;
};
*/
