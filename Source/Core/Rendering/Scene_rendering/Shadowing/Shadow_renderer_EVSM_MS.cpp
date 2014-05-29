#include "Shadow_renderer_EVSM_MS.hpp"
#include "Rendering/Rendering_tool.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Input_layout.hpp"
#include "Rendering/Scene_rendering/Surface_collector.hpp"
#include "Rendering/Vertex_format.hpp"
#include "Resources/Resource_manager.hpp"
#include "Scene/Surface.hpp"

#include <iostream>

namespace rendering
{

bool Shadow_renderer_EVSM_MS::init(const uint2& dimensions, Rendering_tool& rendering_tool, Resource_manager& resource_manager, Constant_buffer_cache& /*constant_buffer_cache*/)
{
	auto& device = rendering_tool.device();
	auto& render_target_cache = rendering_tool.render_target_cache();

	Texture_description texture_description;
	texture_description.type = Texture_description::Type::Texture_2D_multisample;
	texture_description.format = Data_format::D32_Float;
	texture_description.dimensions.xy = dimensions;
	texture_description.num_samples = 4;
	texture_description.shader_resource = true;

	depth_ = render_target_cache.get_depth_stencil_shader_resource_view(texture_description);
	if (!depth_)
	{
		return false;
	}

	generate_shadow_.framebuffer = device.create_framebuffer();
	if (!generate_shadow_.framebuffer)
	{
		return false;
	}

	generate_shadow_.framebuffer->set_render_targets(depth_->depth_stencil_view());
	if (!generate_shadow_.framebuffer->is_valid())
	{
		return false;
	}

	resolve_shadow_.framebuffer = device.create_framebuffer();
	if (!resolve_shadow_.framebuffer)
	{
		return false;
	}

	resolve_shadow_.framebuffer->set_render_targets(shadow_map_->render_target_view());
	if (!resolve_shadow_.framebuffer->is_valid())
	{
		return false;
	}

	generate_shadow_.effect = resource_manager.load<Effect>("Effects/Shadowing/Generate_shadow_map_EVSM_MS.effect");
	if (!generate_shadow_.effect)
	{
		return false;
	}

	generate_shadow_.technique            = generate_shadow_.effect->technique("Generate_shadow_map");
	generate_shadow_.alpha_test_technique = generate_shadow_.effect->technique("Generate_shadow_map_alpha_test");

	auto& vertex_layout_cache = rendering_tool.vertex_layout_cache();

	generate_shadow_.input_layout = vertex_layout_cache.input_layout(*Vertex_position3x32::vertex_layout_description(), generate_shadow_.technique->program()->signature());
	if (!generate_shadow_.input_layout)
	{
		return false;
	}

	generate_shadow_.alpha_test_input_layout = vertex_layout_cache.input_layout(*Vertex_position3x32_tex_coord2x32::vertex_layout_description(), generate_shadow_.alpha_test_technique->program()->signature());
	if (!generate_shadow_.alpha_test_input_layout)
	{
		return false;
	}

	if (!generate_shadow_.change_per_object.init(generate_shadow_.effect, "Change_per_object"))
	{
		return false;
	}

	if (!generate_shadow_.change_per_view.init(generate_shadow_.effect, "Change_per_view"))
	{
		return false;
	}

	resolve_shadow_.effect = resource_manager.load<Effect>("Effects/Shadowing/Resolve_shadow_map_EVSM_MS.effect");
	if (!resolve_shadow_.effect)
	{
		return false;
	}

	resolve_shadow_.technique_linear = resolve_shadow_.effect->technique("Resolve_linear");
	resolve_shadow_.technique_not_linear = resolve_shadow_.effect->technique("Resolve_not_linear");

	resolve_shadow_.input_layout = vertex_layout_cache.input_layout(*Vertex_position2x32::vertex_layout_description(), resolve_shadow_.technique_linear->program()->signature());
	if (!resolve_shadow_.input_layout)
	{
		return false;
	}

	if (!resolve_shadow_.change_per_view.init(resolve_shadow_.effect, "Change_per_view"))
	{
		return false;
	}

	return create_render_states(rendering_tool.render_state_cache());
}

void Shadow_renderer_EVSM_MS::generate_shadow(const float4x4& view_projection, const std::vector<Render_surface>& surfaces, const Viewport& viewport, Rendering_device& device)
{
	device.set_rasterizer_state(rasterizer_state_cull_back_);
	device.set_depth_stencil_state(generate_shadow_.ds_state, 0);
	device.set_blend_state(generate_shadow_.blend_state);

	device.set_viewports(1, &viewport);
	device.set_framebuffer(generate_shadow_.framebuffer);
	device.clear_depth_stencil(generate_shadow_.framebuffer, 1.f, 0);

	generate_shadow_.effect->use(device);

	generate_shadow_.change_per_view.data().view_projection = view_projection;
	generate_shadow_.change_per_view.update(device);

	previous_material_ = nullptr;
	previous_technique_ = nullptr;
	previous_rasterizer_state_ = false;
	const Index_buffer* previous_ib = nullptr;
	const float4x4* previous_world_transformation = nullptr;

	for (const auto& s : surfaces)
	{
		if (s.surface->world != previous_world_transformation)
		{
			const float4x4& transform = *s.surface->world;
			generate_shadow_.change_per_object.data().world = transform;
			generate_shadow_.change_per_object.update(device);

			previous_world_transformation = s.surface->world;
		}

		const scene::Material* material = s.surface->material;

		bool input_layout_changed = prepare_material(material, device);

		if (input_layout_changed || s.surface->index_buffer != previous_ib)
		{
			device.set_vertex_buffers(s.surface->vd->num_streams(), s.surface->vertex_buffers, s.surface->vd->strides());
			device.set_index_buffer(s.surface->index_buffer);

			previous_ib = s.surface->index_buffer;
		}

		device.draw_indexed(s.surface->num_indices, s.surface->start_index);
	}
}

bool Shadow_renderer_EVSM_MS::prepare_material(const scene::Material* material, Rendering_device& device)
{
	if (previous_material_ == material)
	{
		return false;
	}

	device.set_shader_resources(1, &material->textures()[0]);

	bool two_sided = material->is_two_sided();

	if (previous_rasterizer_state_ != two_sided)
	{
		device.set_rasterizer_state(two_sided ? rasterizer_state_cull_none_ : rasterizer_state_cull_back_);
		previous_rasterizer_state_ = two_sided;
	}

	bool alpha_test = material->has_alpha_transparency();

	const Effect_technique* technique = alpha_test ? generate_shadow_.alpha_test_technique : generate_shadow_.technique;

	bool input_layout_changed = false;

	if (previous_technique_ != technique)
	{
		device.set_input_layout(alpha_test ? generate_shadow_.alpha_test_input_layout : generate_shadow_.input_layout);

		technique->use();

		previous_technique_ = technique;

		input_layout_changed = true;
	}

	previous_material_ = material;

	return input_layout_changed;
}

void Shadow_renderer_EVSM_MS::resolve_shadow_linear(const Handle<Framebuffer> target, const Handle<Shader_resource_view>& source, const float2& depth_clamp, const Viewport& viewport, Rendering_tool rendering_tool)
{
	Rendering_device& device = rendering_tool.device();

	device.set_rasterizer_state(resolve_shadow_.rasterizer_state);
	device.set_depth_stencil_state(resolve_shadow_.ds_state, 0);
	device.set_blend_state(resolve_shadow_.blend_state);

	device.set_viewports(1, &viewport);
	device.set_framebuffer(target);

	device.set_shader_resources(1, &source);

	device.set_input_layout(resolve_shadow_.input_layout);

	resolve_shadow_.effect->use(device);

	resolve_shadow_.change_per_view.data().depth_clamp = depth_clamp;
	resolve_shadow_.change_per_view.update(device);

	resolve_shadow_.technique_linear->use();

	rendering_tool.render_fullscreen_effect();
}

void Shadow_renderer_EVSM_MS::resolve_shadow_not_linear(const Handle<Framebuffer> target, const Handle<Shader_resource_view>& source, const float2& linear_depth_projection, const Viewport& viewport, Rendering_tool rendering_tool)
{
	Rendering_device& device = rendering_tool.device();

	device.set_rasterizer_state(resolve_shadow_.rasterizer_state);
	device.set_depth_stencil_state(resolve_shadow_.ds_state, 0);
	device.set_blend_state(resolve_shadow_.blend_state);

	device.set_viewports(1, &viewport);
	device.set_framebuffer(target);

	device.set_shader_resources(1, &source);

	device.set_input_layout(resolve_shadow_.input_layout);

	resolve_shadow_.effect->use(device);

	resolve_shadow_.change_per_view.data().depth_clamp = linear_depth_projection;
	resolve_shadow_.change_per_view.update(device);

	resolve_shadow_.technique_not_linear->use();

	rendering_tool.render_fullscreen_effect();
}

bool Shadow_renderer_EVSM_MS::create_render_states(Render_state_cache& cache)
{
	Rasterizer_state::Description rasterizer_description;

	rasterizer_description.cull_mode = Rasterizer_state::Description::Cull_mode::Back;
	rasterizer_description.multisample_enable = true;
	rasterizer_state_cull_back_ = cache.get_rasterizer_state(rasterizer_description);
	if (!rasterizer_state_cull_back_)
	{
		return false;
	}

	rasterizer_description.cull_mode = Rasterizer_state::Description::Cull_mode::None;
	rasterizer_description.multisample_enable = true;
	rasterizer_state_cull_none_ = cache.get_rasterizer_state(rasterizer_description);
	if (!rasterizer_state_cull_none_)
	{
		return false;
	}

	rasterizer_description.cull_mode = Rasterizer_state::Description::Cull_mode::Back;
	rasterizer_description.multisample_enable = false;
	resolve_shadow_.rasterizer_state = cache.get_rasterizer_state(rasterizer_description);
	if (!resolve_shadow_.rasterizer_state)
	{
		return false;
	}

	Depth_stencil_state::Description ds_description;

	ds_description.depth_enable = true;
	ds_description.depth_write_mask = true;
	ds_description.comparison_func = Depth_stencil_state::Description::Comparison::Less;
	ds_description.stencil_enable = false;
	ds_description.front_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.comparison_func = Depth_stencil_state::Description::Comparison::Equal;
	ds_description.back_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.comparison_func = Depth_stencil_state::Description::Comparison::Equal;
	generate_shadow_.ds_state = cache.get_depth_stencil_state(ds_description);
	if (!generate_shadow_.ds_state)
	{
		return false;
	}

	ds_description.depth_enable = false;
	ds_description.depth_write_mask = false;
	ds_description.comparison_func = Depth_stencil_state::Description::Comparison::Less;
	ds_description.stencil_enable = false;
	ds_description.front_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.comparison_func = Depth_stencil_state::Description::Comparison::Equal;
	ds_description.back_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.comparison_func = Depth_stencil_state::Description::Comparison::Always;
	resolve_shadow_.ds_state = cache.get_depth_stencil_state(ds_description);
	if (!resolve_shadow_.ds_state)
	{
		return false;
	}

	Blend_state::Description blend_description;

	blend_description.independent_blend_enable = false;
	blend_description.render_targets[0].blend_enable            = false;
	blend_description.render_targets[0].source_blend            = Blend_state::Description::Blend::One;
	blend_description.render_targets[0].destination_blend       = Blend_state::Description::Blend::One;
	blend_description.render_targets[0].blend_op                = Blend_state::Description::Blend_op::Add;
	blend_description.render_targets[0].source_blend_alpha      = Blend_state::Description::Blend::Zero;
	blend_description.render_targets[0].destination_blend_alpha = Blend_state::Description::Blend::Zero;
	blend_description.render_targets[0].blend_op_alpha          = Blend_state::Description::Blend_op::Add;
	blend_description.render_targets[0].color_write_mask        = Blend_state::Description::Color_write_mask::Red;
	generate_shadow_.blend_state = cache.get_blend_state(blend_description);
	if (!generate_shadow_.blend_state)
	{
		return false;
	}

	blend_description.independent_blend_enable = false;
	blend_description.render_targets[0].blend_enable            = false;
	blend_description.render_targets[0].source_blend            = Blend_state::Description::Blend::One;
	blend_description.render_targets[0].destination_blend       = Blend_state::Description::Blend::One;
	blend_description.render_targets[0].blend_op                = Blend_state::Description::Blend_op::Add;
	blend_description.render_targets[0].source_blend_alpha      = Blend_state::Description::Blend::Zero;
	blend_description.render_targets[0].destination_blend_alpha = Blend_state::Description::Blend::Zero;
	blend_description.render_targets[0].blend_op_alpha          = Blend_state::Description::Blend_op::Add;
	blend_description.render_targets[0].color_write_mask        = Blend_state::Description::Color_write_mask::All;
	resolve_shadow_.blend_state = cache.get_blend_state(blend_description);
	if (!resolve_shadow_.blend_state)
	{
		return false;
	}

	return true;
}

}
