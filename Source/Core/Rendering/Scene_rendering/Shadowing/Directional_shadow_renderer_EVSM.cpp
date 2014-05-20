#include "Directional_shadow_renderer_EVSM.hpp"
#include "Rendering/Rendering_tool.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Effect/Effect.hpp"
#include "Rendering/Vertex_format.hpp"
#include "Rendering/Input_layout.hpp"
#include "Rendering/Rendering_context.hpp"
#include "Rendering/Scene_rendering/Surface_collector.hpp"
#include "Resources/Resource_manager.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Math/Intersection/Polyhedron.hpp"

#include <String/String.hpp>
#include <iostream>

namespace rendering
{

Directional_shadow_renderer_EVSM::Directional_shadow_renderer_EVSM(Rendering_tool& rendering_tool, Surface_collector& surface_collector, const uint2& size) :
	Directional_shadow_renderer(rendering_tool, surface_collector, size)
{}

bool Directional_shadow_renderer_EVSM::init(Resource_manager& resource_manager, Constant_buffer_cache& /*constant_buffer_cache*/)
{
	auto& cache = rendering_tool_.get_render_target_cache();

	Texture_description texture_description;
	texture_description.type = Texture_description::Type::Texture_2D;
	texture_description.format = Data_format::D32_Float;
	texture_description.dimensions.xy = shadow_map_dimensions_;
	texture_description.shader_resource = false;

	depth_ = rendering_tool_.get_device().create_depth_stencil_view(texture_description);

	if (!depth_)
	{
		return false;
	}

	texture_description.format = Data_format::R32G32B32A32_Float;
	texture_description.shader_resource = true;
	shadow_map_ = cache.get_render_target_shader_resource_view(texture_description);

	if (!shadow_map_)
	{
		return false;
	}

	generate_shadow_.framebuffer = rendering_tool_.get_device().create_framebuffer();

	if (!generate_shadow_.framebuffer)
	{
		return false;
	}

	generate_shadow_.framebuffer->set_render_targets(shadow_map_->get_render_target_view(), depth_);

	if (!generate_shadow_.framebuffer->is_valid())
	{
		return false;
	}

	apply_shadow_.framebuffer = rendering_tool_.get_device().create_framebuffer();

	if (!apply_shadow_.framebuffer)
	{
		return false;
	}

	generate_shadow_.effect = resource_manager.load<Effect>("Effects/Shadowing/Generate_shadow_map_EVSM.effect");
	if (!generate_shadow_.effect)
	{
		return false;
	}

	generate_shadow_.technique            = generate_shadow_.effect->get_technique("Generate_shadow_map");
	generate_shadow_.alpha_test_technique = generate_shadow_.effect->get_technique("Generate_shadow_map_alpha_test");

	generate_shadow_.input_layout = rendering_tool_.get_vertex_layout_cache().get_input_layout(*Vertex_position3x32::vertex_layout_description(), generate_shadow_.technique->get_program()->get_signature());
	if (!generate_shadow_.input_layout)
	{
		return false;
	}

	generate_shadow_.alpha_test_input_layout = rendering_tool_.get_vertex_layout_cache().get_input_layout(*Vertex_position3x32_tex_coord2x32::vertex_layout_description(), generate_shadow_.alpha_test_technique->get_program()->get_signature());
	if (!generate_shadow_.alpha_test_input_layout)
	{
		return false;
	}

	if (!generate_shadow_.change_per_cascade.init(generate_shadow_.effect, "Change_per_cascade"))
	{
		return false;
	}

	if (!generate_shadow_.change_per_object.init(generate_shadow_.effect, "Change_per_object"))
	{
		return false;
	}

	apply_shadow_.effect = resource_manager.load<Effect>("Effects/Shadowing/Apply_shadow_map_EVSM.effect");
	if (!apply_shadow_.effect)
	{
		return false;
	}

	apply_shadow_.technique        = apply_shadow_.effect->get_technique("Apply_shadow_map");
	apply_shadow_.volume_technique = apply_shadow_.effect->get_technique("Apply_shadow_map_volume");

	apply_shadow_.input_layout = rendering_tool_.get_vertex_layout_cache().get_input_layout(*Vertex_position2x32_tex_coord2x32::vertex_layout_description(), apply_shadow_.technique->get_program()->get_signature());
	if (!apply_shadow_.input_layout)
	{
		return false;
	}

	apply_shadow_.volume_input_layout = rendering_tool_.get_vertex_layout_cache().get_input_layout(*Vertex_position3x32::vertex_layout_description(), apply_shadow_.volume_technique->get_program()->get_signature());
	if (!apply_shadow_.volume_input_layout)
	{
		return false;
	}

	if (!apply_shadow_.filter_kernel.init(apply_shadow_.effect, "Filter_kernel"))
	{
		return false;
	}

	if (!apply_shadow_.change_per_camera.init(apply_shadow_.effect, "Change_per_camera"))
	{
		return false;
	}

	if (!apply_shadow_.change_per_cascade.init(apply_shadow_.effect, "Change_per_cascade"))
	{
		return false;
	}

	if (!volume_.init(rendering_tool_))
	{
		return false;
	}

	for (uint32_t i = 0; i < filter_kernel_size_; ++i)
	{
		apply_shadow_.filter_kernel.get_data().filter_kernel[i] = filter_kernel_[i];
	}

	apply_shadow_.filter_kernel.update(rendering_tool_.get_device());

	return create_render_states();
}

Handle<Shader_resource_view>& Directional_shadow_renderer_EVSM::get_shadow_map()
{
	return shadow_map_->get_shader_resource_view();
}

Handle<Shader_resource_view>& Directional_shadow_renderer_EVSM::get_white_buffer()
{
	return white_buffer_->get_shader_resource_view();
}

void Directional_shadow_renderer_EVSM::prepare_rendering(const Rendering_context& context)
{
	const auto& camera = context.get_camera();

	Rendering_device& device = rendering_tool_.get_device();

	const float3* rays = camera.get_view_rays_vs();
	const float2& linear_depth_projection = camera.get_linear_depth_projection();

	auto& change_per_camera_data = apply_shadow_.change_per_camera.get_data();
	change_per_camera_data.rays[0] = float4(rays[0], linear_depth_projection.x);
	change_per_camera_data.rays[1] = float4(rays[1], linear_depth_projection.y);
	change_per_camera_data.rays[2] = float4(rays[2], 1.f);
	change_per_camera_data.view_projection = camera.get_view_projection();
	change_per_camera_data.view = camera.get_view();
	change_per_camera_data.inverse_view = invert(camera.get_view());
	apply_shadow_.change_per_camera.update(device);
}

void Directional_shadow_renderer_EVSM::render_cascade(const Rendering_context& context, const Cascade_data& cascade_data)
{
	generate_shadow(cascade_data);

	apply_shadow(context, cascade_data);
}

void Directional_shadow_renderer_EVSM::generate_shadow(const Cascade_data& cascade_data)
{
	Rendering_device& device = rendering_tool_.get_device();

	device.set_rasterizer_state(rasterizer_state_cull_back_);
	device.set_depth_stencil_state(generate_shadow_.ds_state, 0);
	device.set_blend_state(generate_shadow_.blend_state);

	device.set_viewports(1, &viewport_);
	device.set_framebuffer(generate_shadow_.framebuffer);
//	Color4 clear_color(10000000.f, 10000000.f, 10000000.f, 1000000.f);
//	device.clear_color(generate_shadow_.framebuffer, 1, &clear_color);
	device.clear_depth_stencil(generate_shadow_.framebuffer, 1.f, 0);

	device.set_input_layout(generate_shadow_.input_layout);

	generate_shadow_.effect->use(device);

	generate_shadow_.change_per_cascade.get_data().depth_clamp = cascade_data.depth_clamp;
	generate_shadow_.change_per_cascade.update(device);

	generate_shadow_.technique->use();

	previous_material_ = nullptr;
	previous_technique_ = nullptr;
	previous_rasterizer_state_ = false;
	const Index_buffer* previous_ib = nullptr;
	const float4x4* previous_world_transformation = nullptr;

	const std::vector<Render_surface>& surfaces = surface_collector_.get_surfaces();

	for (const auto& s : surfaces)
	{
		if (s.surface->world != previous_world_transformation)
		{
			const float4x4& transform = *s.surface->world;
			generate_shadow_.change_per_object.get_data().world_view_projection = transform * cascade_data.shadow_view_projection;
			generate_shadow_.change_per_object.update(device);

			previous_world_transformation = s.surface->world;
		}

		const scene::Material* material = s.surface->material;

		bool input_layout_changed = prepare_material(material);

		if (input_layout_changed || s.surface->index_buffer != previous_ib)
		{
			device.set_vertex_buffers(s.surface->vd->get_num_streams(), s.surface->vertex_buffers, s.surface->vd->get_strides());
			device.set_index_buffer(s.surface->index_buffer);

			previous_ib = s.surface->index_buffer;
		}

		device.draw_indexed(s.surface->num_indices, s.surface->start_index);
	}
}

bool Directional_shadow_renderer_EVSM::prepare_material(const scene::Material* material)
{
	if (previous_material_ == material)
	{
		return false;
	}

	auto& device = rendering_tool_.get_device();

	device.set_shader_resources(1, &material->get_textures()[0]);

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

void Directional_shadow_renderer_EVSM::apply_shadow(const Rendering_context& context, const Cascade_data& cascade_data)
{
	Rendering_device& device = rendering_tool_.get_device();

	device.set_viewports(1, &context.get_viewport());
	device.set_framebuffer(apply_shadow_.framebuffer);

	device.set_shader_resources(1, &deferred_depth_);
	device.set_shader_resources(1, &shadow_map_->get_shader_resource_view(), 1);

	apply_shadow_.effect->use(device);

	device.set_blend_state(apply_shadow_.blend_state);

	const auto& camera = context.get_camera();

	auto& change_per_cascade_data = apply_shadow_.change_per_cascade.get_data();
	change_per_cascade_data.shadow_transform = invert(camera.get_view()) * cascade_data.shadow_view_projection * scene::Light::get_texture_transform();
	change_per_cascade_data.depth_clamp = cascade_data.depth_clamp;

	if (cascade_data.last_cascade)
	{
		device.set_input_layout(apply_shadow_.input_layout);

		device.set_rasterizer_state(apply_shadow_.rasterizer_state);
		device.set_depth_stencil_state(apply_shadow_.ds_state, 1);

		apply_shadow_.change_per_cascade.update(device);

		apply_shadow_.technique->use();

		rendering_tool_.render_fullscreen_effect();
	}
	else
	{
		device.set_input_layout(apply_shadow_.volume_input_layout);

		device.set_rasterizer_state(camera.is_upside_down() ? apply_shadow_.rasterizer_state_upside_down : apply_shadow_.rasterizer_state);
		device.set_depth_stencil_state(apply_shadow_.volume_ds_state, 1);

		change_per_cascade_data.world = cascade_data.cascade_world;
		apply_shadow_.change_per_cascade.update(device);

		apply_shadow_.volume_technique->use();

		volume_.render(rendering_tool_);
	}
}

bool Directional_shadow_renderer_EVSM::on_resize_targets(const uint2& size, const Handle<Depth_stencil_shader_resource_view>& depth_stencil)
{
	auto& cache = rendering_tool_.get_render_target_cache();

	Texture_description texture_description;
	texture_description.type = Texture_description::Type::Texture_2D;
	texture_description.dimensions.xy = size;
	texture_description.format = Data_format::R8_UNorm;
	white_buffer_ = cache.get_render_target_shader_resource_view(texture_description);

	if (!white_buffer_)
	{
		return false;
	}

	apply_shadow_.framebuffer->set_render_targets(white_buffer_->get_render_target_view(), depth_stencil->get_depth_stencil_view());

	if (!apply_shadow_.framebuffer->is_valid())
	{
		return false;
	}

    return true;
}

bool Directional_shadow_renderer_EVSM::create_render_states()
{
	Rasterizer_state::Description rasterizer_description;

	rasterizer_description.cull_mode = Rasterizer_state::Description::Cull_mode::Back;
	rasterizer_state_cull_back_ = rendering_tool_.get_render_state_cache().get_rasterizer_state(rasterizer_description);
	if (!rasterizer_state_cull_back_)
	{
		return false;
	}

	apply_shadow_.rasterizer_state = rasterizer_state_cull_back_;

	rasterizer_description.cull_mode = Rasterizer_state::Description::Cull_mode::Front;
	apply_shadow_.rasterizer_state_upside_down = rendering_tool_.get_render_state_cache().get_rasterizer_state(rasterizer_description);
	if (!apply_shadow_.rasterizer_state_upside_down)
	{
		return false;
	}

	rasterizer_description.cull_mode = Rasterizer_state::Description::Cull_mode::None;
	rasterizer_state_cull_none_ = rendering_tool_.get_render_state_cache().get_rasterizer_state(rasterizer_description);
	if (!rasterizer_state_cull_none_)
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
	generate_shadow_.ds_state = rendering_tool_.get_render_state_cache().get_depth_stencil_state(ds_description);
	if (!generate_shadow_.ds_state)
	{
		return false;
	}

	ds_description.depth_enable = false;
	ds_description.depth_write_mask = false;
	ds_description.comparison_func = Depth_stencil_state::Description::Comparison::Less;
	ds_description.stencil_enable = true;
	ds_description.front_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Decrement_saturate;
	ds_description.front_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.comparison_func = Depth_stencil_state::Description::Comparison::Equal;
	ds_description.back_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.comparison_func = Depth_stencil_state::Description::Comparison::Always;
	apply_shadow_.ds_state = rendering_tool_.get_render_state_cache().get_depth_stencil_state(ds_description);
	if (!apply_shadow_.ds_state)
	{
		return false;
	}

	ds_description.depth_enable = true;
	ds_description.depth_write_mask = false;
	ds_description.comparison_func = Depth_stencil_state::Description::Comparison::Greater;
	ds_description.stencil_enable = true;
	ds_description.front_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Increment;
	ds_description.front_face.comparison_func = Depth_stencil_state::Description::Comparison::Equal;
	ds_description.back_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.comparison_func = Depth_stencil_state::Description::Comparison::Always;
	apply_shadow_.volume_ds_state = rendering_tool_.get_render_state_cache().get_depth_stencil_state(ds_description);
	if (!apply_shadow_.volume_ds_state)
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
	blend_description.render_targets[0].color_write_mask        = Blend_state::Description::Color_write_mask::All;

	generate_shadow_.blend_state = rendering_tool_.get_render_state_cache().get_blend_state(blend_description);
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
	blend_description.render_targets[0].color_write_mask        = Blend_state::Description::Color_write_mask::Red;

	apply_shadow_.blend_state = rendering_tool_.get_render_state_cache().get_blend_state(blend_description);
	if (!apply_shadow_.blend_state)
	{
		return false;
	}

	return true;
}

}
