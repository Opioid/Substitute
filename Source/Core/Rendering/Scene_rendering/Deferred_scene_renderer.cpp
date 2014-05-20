#include "Deferred_scene_renderer.hpp"
#include "Surface_collector.hpp"
#include "Rendering/Rendering_tool.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Vertex_format.hpp"
#include "Rendering/Input_layout.hpp"
#include "Surrounding/Surrounding_renderer.hpp"
#include "Math/Intersection/Sphere.hpp"
#include "Math/Intersection/OBB.hpp"
#include "Rendering/Rendering_context.hpp"
#include "Rendering/Effect/Effect_provider.hpp"
#include "Resources/Resource_manager.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Model.hpp"
#include "Scene/Actor.hpp"
#include "Scene/Material.hpp"
#include "Scene/Light/Irradiance_volume.hpp"
#include "Scene/Light/Light_probe.hpp"

#include <iostream>

namespace rendering
{

Deferred_scene_renderer::Deferred_scene_renderer(Rendering_tool& rendering_tool,
												 Surface_collector& surface_collector,
												 Surrounding_renderer& surrounding_renderer,
												 Directional_shadow_renderer& directional_shadow_renderer,
												 Spot_shadow_renderer& spot_shadow_renderer) :
	Main_scene_renderer(rendering_tool, surface_collector, surrounding_renderer),
	lighting_renderer_(rendering_tool, directional_shadow_renderer, spot_shadow_renderer),
	previous_material_(nullptr)
{}

bool Deferred_scene_renderer::init(Resource_manager& resource_manager, Constant_buffer_cache& /*constant_buffer_cache*/)
{
	framebuffer_ = rendering_tool_.get_device().create_framebuffer();

	if (!framebuffer_)
	{
		return false;
	}

	Flags flags;
	flags.set(Effect_provider::Options::Use_custom_constant_buffers, true);

	effect_ = resource_manager.load<Effect>("Effects/Deferred_renderer_base.effect", flags);
	if (!effect_)
	{
		return false;
	}

	input_layout_ = rendering_tool_.get_vertex_layout_cache().get_input_layout(*Vertex_position3x32_tex_coord2x32_normal1010102::vertex_layout_description(), effect_->get_technique(3)->get_program()->get_signature());
	if (!input_layout_)
	{
		return false;
	}

	auto& device = rendering_tool_.get_device();

	if (!change_per_camera_.init(effect_, "Change_per_camera"))
	{
		return false;
	}

	Constant_buffer_adapter* change_per_camera_adapter = change_per_camera_.get_adapter();
	Handle<Constant_buffer> change_per_camera_buffer = device.create_constant_buffer(change_per_camera_adapter->get_num_bytes());
	change_per_camera_adapter->set_constant_buffer(change_per_camera_buffer);

	constant_buffer_cache_.set_constant_buffer("Change_per_camera", change_per_camera_buffer);

	if (!change_per_object_.init(effect_, "Change_per_object"))
	{
		return false;
	}

	if (!change_per_material_.init(effect_, "Change_per_material"))
	{
		return false;
	}

	effect_->create_default_constant_buffers(device);

	if (!lighting_renderer_.init(resource_manager, constant_buffer_cache_))
	{
		return false;
	}

	return create_render_states();
}

bool Deferred_scene_renderer::on_resize_targets(const uint2& size, const Handle<Depth_stencil_shader_resource_view>& depth_stencil)
{
	auto& cache = rendering_tool_.get_render_target_cache();

	Texture_description texture_description;
	texture_description.type = Texture_description::Type::Texture_2D;
	texture_description.dimensions.xy = size;
	texture_description.format = Data_format::R16G16_UNorm;
	texture_description.shader_resource = true;
	normal_target_ = cache.get_render_target_shader_resource_view(texture_description);

	if (!normal_target_)
	{
		return false;
	}

	texture_description.format = Data_format::R8G8B8A8_UNorm_sRGB;
	color_target_ = cache.get_render_target_shader_resource_view(texture_description);

	if (!color_target_)
	{
		return false;
	}

	texture_description.format = Data_format::R8G8B8A8_UNorm;
	surface_target_ = cache.get_render_target_shader_resource_view(texture_description);

	if (!surface_target_)
	{
		return false;
	}

	framebuffer_->set_render_targets({ normal_target_->get_render_target_view(), color_target_->get_render_target_view(), surface_target_->get_render_target_view() },
									 depth_stencil->get_depth_stencil_view());

	if (!framebuffer_->is_valid())
	{
		return false;
	}

	if (!lighting_renderer_.resize_targets(size, depth_stencil))
	{
		return false;
	}

	lighting_renderer_.set_deferred_textures(depth_stencil->get_shader_resource_view(),
											 normal_target_->get_shader_resource_view(),
											 color_target_->get_shader_resource_view(),
											 surface_target_->get_shader_resource_view());

	return true;
}

void Deferred_scene_renderer::render(const scene::Scene& scene, const Rendering_context& context)
{
	auto& device = rendering_tool_.get_device();

	device.set_primitive_topology(Primitive_topology::Triangle_list);

	auto& options = context.get_options();

	if (options.is_set(Rendering_context::Options::Render_actors)
	||  options.is_set(Rendering_context::Options::Render_static_geometry))
	{
		device.set_viewports(1, &context.get_viewport());
		device.set_framebuffer(framebuffer_);

		device.set_input_layout(input_layout_);

		const auto& camera = context.get_camera();

		if (options.is_set(Rendering_context::Options::Face_culling))
		{
			base_rasterizer_states_[0] = camera.is_upside_down() ? rasterizer_state_cull_front_ : rasterizer_state_cull_back_;
		}
		else
		{
			base_rasterizer_states_[0] = rasterizer_state_cull_none_;
		}

		base_rasterizer_states_[1] = rasterizer_state_cull_none_;

		device.set_rasterizer_state(base_rasterizer_states_[0]);
		device.set_depth_stencil_state(base_ds_state_, 1);
		device.set_blend_state(base_blend_state_);

		effect_->use(device);

		const float3* rays = camera.get_view_rays_vs();
		const float2& linear_depth_projection = camera.get_linear_depth_projection();
		auto& change_per_camera_data = change_per_camera_.get_data();
		change_per_camera_data.rays[0] = float4(rays[0], linear_depth_projection.x);
		change_per_camera_data.rays[1] = float4(rays[1], linear_depth_projection.y);
		change_per_camera_data.rays[2] = float4(rays[2], 1.f);
		change_per_camera_data.view_projection = camera.get_view_projection();
		change_per_camera_data.view = camera.get_view();
		change_per_camera_data.inverse_view = invert(camera.get_view());
		change_per_camera_.update(device);

		previous_material_ = nullptr;
		previous_technique_ = 0xFFFFFFFF;
		previous_rasterizer_state_ = 0;

		const Index_buffer* previous_ib = nullptr;
		const float4x4* previous_world_transformation = nullptr;

		surface_collector_.collect(
			scene, camera.get_world_position(), camera.get_frustum(),
			options.is_set(Rendering_context::Options::Render_actors),
			options.is_set(Rendering_context::Options::Render_static_geometry));

		const std::vector<Render_surface>& surfaces = surface_collector_.get_surfaces();

		for (const auto& s : surfaces)
		{
			if (s.surface->world != previous_world_transformation)
			{
				const float4x4& transform = *s.surface->world;
				change_per_object_.get_data().world = transform;
				change_per_object_.update(device);

				previous_world_transformation = s.surface->world;
			}

			const scene::Material* material = s.surface->material;

			prepare_material(material);

			if (s.surface->index_buffer != previous_ib)
			{
				device.set_vertex_buffers(s.surface->vd->get_num_streams(), s.surface->vertex_buffers, s.surface->vd->get_strides());
				device.set_index_buffer(s.surface->index_buffer);
				previous_ib = s.surface->index_buffer;
			}
			device.draw_indexed(s.surface->num_indices, s.surface->start_index);

		}

		render_deferred_effects(scene, context);
	}

	device.set_rasterizer_state(rasterizer_state_cull_back_);

	if (options.is_set(Rendering_context::Options::Render_surrounding))
	{
		surrounding_renderer_.render(scene, context);
	}
	else
	{
		surrounding_renderer_.clear(color3::black, context);
	}
}

void Deferred_scene_renderer::render_deferred_effects(const scene::Scene& scene, const Rendering_context& context)
{
	lighting_renderer_.render(scene, context);
}

void Deferred_scene_renderer::prepare_material(const scene::Material* material)
{
	if (previous_material_ == material)
	{
		return;
	}

	auto& device = rendering_tool_.get_device();

	uint32_t rasterizer_state = material->is_two_sided() ? 1 : 0;
	if (previous_rasterizer_state_ != rasterizer_state)
	{
		device.set_rasterizer_state(base_rasterizer_states_[rasterizer_state]);
		previous_rasterizer_state_ = rasterizer_state;
	}

	auto& change_per_mater_data = change_per_material_.get_data();
	change_per_mater_data.color_and_emissive_factor = material->get_color_and_emissive_factor();
	change_per_mater_data.metallic_and_roughness = material->get_metallic_and_roughness();
	change_per_mater_data.height_scale = material->get_height_scale();
	change_per_material_.update(device);

	device.set_shader_resources(static_cast<uint32_t>(scene::Material::Sampler::Enum_count), material->get_textures());

	uint32_t technique = static_cast<uint32_t>(material->get_technique());
	if (previous_technique_ != technique)
	{
		effect_->get_technique(technique)->use();
		previous_technique_ = technique;
	}

	previous_material_ = material;
}

bool Deferred_scene_renderer::create_render_states()
{
	Rasterizer_state::Description rasterizer_description;
	rasterizer_description.front_ccw = false;
	rasterizer_description.cull_mode = Rasterizer_state::Description::Cull_mode::Back;
	rasterizer_state_cull_back_ = rendering_tool_.get_render_state_cache().get_rasterizer_state(rasterizer_description);
	if (!rasterizer_state_cull_back_)
	{
		return false;
	}

	rasterizer_description.front_ccw = true;
	rasterizer_description.cull_mode = Rasterizer_state::Description::Cull_mode::Back;
	rasterizer_state_cull_front_ = rendering_tool_.get_render_state_cache().get_rasterizer_state(rasterizer_description);
	if (!rasterizer_state_cull_front_)
	{
		return false;
	}

	rasterizer_description.front_ccw = false;
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
	ds_description.stencil_enable = true;
	ds_description.front_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Replace;
	ds_description.front_face.comparison_func = Depth_stencil_state::Description::Comparison::Greater_equal;
	ds_description.back_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Replace;
	ds_description.back_face.comparison_func = Depth_stencil_state::Description::Comparison::Greater_equal;
	base_ds_state_ = rendering_tool_.get_render_state_cache().get_depth_stencil_state(ds_description);
	if (!base_ds_state_)
	{
		return false;
	}

	Blend_state::Description blend_description;
	blend_description.independent_blend_enable = true;
	blend_description.render_targets[0].blend_enable     = false;
	blend_description.render_targets[0].color_write_mask = Blend_state::Description::Color_write_mask::Red | Blend_state::Description::Color_write_mask::Green;
	blend_description.render_targets[1].blend_enable     = false;
	blend_description.render_targets[1].color_write_mask = Blend_state::Description::Color_write_mask::All;
	blend_description.render_targets[2].blend_enable     = false;
	blend_description.render_targets[2].color_write_mask = Blend_state::Description::Color_write_mask::All;

	base_blend_state_ = rendering_tool_.get_render_state_cache().get_blend_state(blend_description);
	if (!base_blend_state_)
	{
		return false;
	}

	return true;
}

}
