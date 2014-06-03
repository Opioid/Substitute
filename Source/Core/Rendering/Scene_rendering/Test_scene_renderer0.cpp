#include "Test_scene_renderer0.hpp"
#include "Surface_collector.hpp"
#include "Rendering/Rendering_tool.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Vertex_format.hpp"
#include "Rendering/Input_layout.hpp"
#include "../Rendering_context.hpp"
#include "../Effect/Effect.hpp"
#include "../../Resources/Resource_manager.hpp"
#include "../../Scene/Camera.hpp"
#include "../../Scene/Scene.hpp"
#include "../../Scene/Model.hpp"
#include "../../Scene/Actor.hpp"
#include "../../Scene/Material.hpp"
#include <iostream>

namespace rendering
{

Test_scene_renderer0::Test_scene_renderer0(Rendering_tool& rendering_tool, Surface_collector& surface_collector, Surrounding_renderer& surrounding_renderer) :
	Main_scene_renderer(rendering_tool, surface_collector, surrounding_renderer),
	previous_material_(nullptr)
{}

bool Test_scene_renderer0::init(Resource_manager& resource_manager, Constant_buffer_cache& /*constant_buffer_cache*/)
{
	effect_ = resource_manager.load<Effect>("Effects/Test_renderer.effect");
	if (!effect_)
	{
		return false;
	}

	input_layout_ = rendering_tool_.vertex_layout_cache().input_layout(*Vertex_position3x32_tex_coord2x32_normal1010102::vertex_layout_description(), effect_->technique(0)->program()->signature());
	if (!input_layout_)
	{
		return false;
	}

	if (!change_per_object_.init(effect_, "Change_per_object"))
	{
		return false;
	}

	return create_render_states();
}

bool Test_scene_renderer0::on_resize_targets(const uint2& /*dimensions*/, const Handle<Depth_stencil_shader_resource_view>& /*depth_stencil*/)
{
	return true;
}

void Test_scene_renderer0::render(const scene::Scene& scene, const Rendering_context& context)
{
	auto& device = rendering_tool_.device();

	device.set_primitive_topology(Primitive_topology::Triangle_list);

	if (context.options().is_set(Rendering_context::Options::Render_actors)
	||  context.options().is_set(Rendering_context::Options::Render_static_geometry))
	{
		device.set_viewports(1, &context.viewport());

		device.set_framebuffer(context.framebuffer());

		device.set_input_layout(input_layout_);

		device.set_rasterizer_state(rasterizer_state_cull_back_);
		device.set_depth_stencil_state(base_ds_state_, 1);
		device.set_blend_state(base_blend_state_);

	//	effect_->use(device);

		previous_material_ = nullptr;
		previous_technique_ = 0xFFFFFFFF;

		const auto& camera = context.camera();

		surface_collector_.collect(
			scene, camera.world_position(), camera.frustum(),
			context.options().is_set(Rendering_context::Options::Render_actors),
			context.options().is_set(Rendering_context::Options::Render_static_geometry));

		const std::vector<Render_surface>& surfaces = surface_collector_.get_surfaces();

		bool first_call = true;

		for (const auto& s : surfaces)
		{
			const float4x4& transform = *s.surface->world;
			auto change_per_object_data = change_per_object_.data();
			change_per_object_data.world_view_projection = transform * camera.view_projection();
			change_per_object_data.world_view = transform * camera.view();
			change_per_object_.update(device);

			const scene::Material* material = s.surface->material;

			prepare_material(material);

		//	if (first_call)
			{
				device.set_vertex_buffers(s.surface->vd->num_streams(), s.surface->vertex_buffers, s.surface->vd->strides());
				device.set_index_buffer(s.surface->index_buffer);
				first_call = false;
			}
			device.draw_indexed(s.surface->num_indices, s.surface->start_index);

		}
	}
}

bool Test_scene_renderer0::prepare_material(const scene::Material* material)
{
	if (previous_material_ == material)
	{
		return false;
	}

	auto& device = rendering_tool_.device();

	device.set_shader_resources(static_cast<uint32_t>(scene::Material::Sampler::Enum_count), material->textures());

	uint32_t technique = 0;
	if (previous_technique_ != technique)
	{
		effect_->technique(technique)->use();
		previous_technique_ = technique;
	}

	previous_material_ = material;

	return true;
}

bool Test_scene_renderer0::create_render_states()
{
	Rasterizer_state::Description rasterizer_description;
	rasterizer_description.cull_mode = Rasterizer_state::Description::Cull_mode::Back;
	rasterizer_state_cull_back_ = rendering_tool_.render_state_cache().rasterizer_state(rasterizer_description);
	if (!rasterizer_state_cull_back_)
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
	ds_description.front_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Replace;
	ds_description.front_face.comparison_func = Depth_stencil_state::Description::Comparison::Greater_equal;
	ds_description.back_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Replace;
	ds_description.back_face.comparison_func = Depth_stencil_state::Description::Comparison::Greater_equal;
	base_ds_state_ = rendering_tool_.render_state_cache().depth_stencil_state(ds_description);
	if (!base_ds_state_)
	{
		return false;
	}

	Blend_state::Description blend_description;
	blend_description.independent_blend_enable = false;
	blend_description.render_targets[0].blend_enable     = false;
	blend_description.render_targets[0].color_write_mask = Blend_state::Description::Color_write_mask::All;

	base_blend_state_ = rendering_tool_.render_state_cache().blend_state(blend_description);
	if (!base_blend_state_)
	{
		return false;
	}

	return true;
}

}
