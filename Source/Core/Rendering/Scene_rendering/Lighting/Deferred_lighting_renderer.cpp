#include "Deferred_lighting_renderer.hpp"
#include "Rendering/Rendering_tool.hpp"
#include "Rendering/Vertex_format.hpp"
#include "Rendering/Input_layout.hpp"
#include "Rendering/Rendering_context.hpp"
#include "Rendering/Effect/Effect_provider.hpp"
#include "Rendering/Scene_rendering/Shadowing/Directional_shadow_renderer.hpp"
#include "Rendering/Scene_rendering/Shadowing/Spot_shadow_renderer.hpp"
#include "Rendering/Constant_buffer_cache.hpp"
#include "Math/Vector.inl"
#include "Math/Matrix.inl"
#include "Math/Intersection/Sphere.hpp"
#include "Math/Intersection/OBB.hpp"
#include "Resources/Resource_manager.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Light/Irradiance_volume.hpp"
#include "Scene/Light/Light_probe.hpp"
#include "Logging/Logging.hpp"

namespace rendering
{

Deferred_lighting_renderer::Deferred_lighting_renderer(Rendering_tool& rendering_tool, Directional_shadow_renderer& directional_shadow_renderer, Spot_shadow_renderer& spot_shadow_renderer) :
	Rendering_object(rendering_tool), directional_shadow_renderer_(directional_shadow_renderer), spot_shadow_renderer_(spot_shadow_renderer)
{}

bool Deferred_lighting_renderer::init(Resource_manager& resource_manager, Constant_buffer_cache& constant_buffer_cache)
{
	Flags<Effect_provider::Options, uint32_t> flags;
	flags.set(Effect_provider::Options::Use_custom_constant_buffers, true);

	effect_ = resource_manager.load<Effect>("Effects/Lighting/Deferred_lighting_renderer.effect", flags.data());
	if (!effect_)
	{
		return false;
	}

	techniques_.emissive_light = effect_->technique("Emissive_light");
	techniques_.irradiance_volume = effect_->technique("Irradiance_volume");
	techniques_.light_probe_specular = effect_->technique("Light_probe_specular");
	techniques_.volume_light_probe_specular = effect_->technique("Volume_light_probe_specular");
	techniques_.directional_light = effect_->technique("Directional_light");
	techniques_.directional_light_with_shadow = effect_->technique("Directional_light_with_shadow");
	techniques_.point_light = effect_->technique("Point_light");
	techniques_.spot_light = effect_->technique("Spot_light");
	techniques_.spot_light_with_shadow = effect_->technique("Spot_light_with_shadow");

	input_layout_ = rendering_tool_.vertex_layout_cache().input_layout(*Vertex_position2x32_tex_coord2x32::vertex_layout_description(), techniques_.directional_light->program()->signature());
	if (!input_layout_)
	{
		return false;
	}

	Constant_buffer_adapter* change_per_camera_adapter = effect_->constant_buffer_adapter("Change_per_camera");
	if (!change_per_camera_adapter)
	{
		return false;
	}

	if (!constant_buffer_cache.connect(change_per_camera_adapter, "Change_per_camera"))
	{
		return false;
	}

	if (!filter_kernel_.init(effect_, "Filter_kernel"))
	{
		return false;
	}

	if (!change_per_light_.init(effect_, "Change_per_light"))
	{
		return false;
	}

	auto& device = rendering_tool_.device();

	effect_->create_default_constant_buffers(device);

	volume_input_layout_ = rendering_tool_.vertex_layout_cache().input_layout(*Vertex_position3x32::vertex_layout_description(), techniques_.point_light->program()->signature());
	if (!volume_input_layout_)
	{
		return false;
	}

	light_2D_texture_offset0_         = effect_->sampler_offset("g_light_2D_map0");
	light_2D_texture_offset1_         = effect_->sampler_offset("g_light_2D_map1");
	light_probe_texture_offset_       = effect_->sampler_offset("g_light_probe_map");
	irradiance_volume_texture_offset_ = effect_->sampler_offset("g_irradiance_volume_map0");

	if (!box_volume_.init(rendering_tool_))
	{
		return false;
	}

	if (!sphere_volume_.init(rendering_tool_))
	{
		return false;
	}

	if (!frustum_volume_.init(rendering_tool_))
	{
		return false;
	}

	if (!directional_shadow_renderer_.init(resource_manager, constant_buffer_cache))
	{
		logging::error("Directional shadow renderer could not be initialized.");

		return false;
	}

	if (!spot_shadow_renderer_.init(resource_manager, constant_buffer_cache))
	{
		logging::error("Spot shadow renderer could not be initialized.");

		return false;
	}

	for (uint32_t i = 0, count = spot_shadow_renderer_.filter_kernel_size(); i < count; ++i)
	{
		filter_kernel_.data().filter_kernel[i] = spot_shadow_renderer_.filter_kernel()[i];
	}

	filter_kernel_.update(device);

	return create_render_states();
}

bool Deferred_lighting_renderer::resize_targets(const uint2& size, const Handle<Depth_stencil_shader_resource_view>& depth_stencil)
{
	if (!directional_shadow_renderer_.resize_targets(size, depth_stencil))
	{
		return false;
	}

	directional_shadow_renderer_.set_deferred_depth(depth_stencil->shader_resource_view());

	if (!spot_shadow_renderer_.resize_targets(size, depth_stencil))
	{
		return false;
	}

	return true;
}

void Deferred_lighting_renderer::set_deferred_textures(const Handle<Shader_resource_view>& depth,
													   const Handle<Shader_resource_view>& normal,
													   const Handle<Shader_resource_view>& color,
													   const Handle<Shader_resource_view>& surface)
{
	deferred_textures_[0] = depth;
	deferred_textures_[1] = normal;
	deferred_textures_[2] = color;
	deferred_textures_[3] = surface;
}

void Deferred_lighting_renderer::render(const scene::Scene& scene, const Rendering_context& context)
{
	const auto& camera = context.camera();

	lighting_rasterizer_states_[0] = camera.is_upside_down() ? rasterizer_state_cull_front_ : rasterizer_state_cull_back_;
	lighting_rasterizer_states_[1] = camera.is_upside_down() ? rasterizer_state_cull_back_  : rasterizer_state_cull_front_;

	Rendering_device& device = rendering_tool_.device();

	device.set_framebuffer(context.framebuffer());

	device.set_shader_resources(4, deferred_textures_);

	effect_->use(device);

	if (context.options().is_set(Rendering_context::Options::Render_emissive_lighting))
	{
		render_emissive_light();
	}

	if (context.options().is_set(Rendering_context::Options::Render_image_based_lighting))
	{
		render_irradiance_volumes(scene, context);

		render_light_probes(scene, context);
	}

	if (context.options().is_set(Rendering_context::Options::Render_analytical_lighting))
	{
		for (auto l : scene.lights())
		{
			const scene::Light& light = *l;

			if (!light.is_visible())
			{
				continue;
			}

			switch (light.type())
			{
			case scene::Light::Type::Directional:
				render_directional_light(light, scene, context);
				break;
			case scene::Light::Type::Point:
				render_point_light(light, scene, context);
				break;
			case scene::Light::Type::Spot:
				render_spot_light(light, scene, context);
				break;
			}
		}
	}
}

void Deferred_lighting_renderer::render_emissive_light()
{
	Rendering_device& device = rendering_tool_.device();

	device.set_input_layout(input_layout_);

	device.set_rasterizer_state(rasterizer_state_cull_back_);
	device.set_depth_stencil_state(lighting_ds_state_, 1);
	device.set_blend_state(lighting_blend_state_);

	techniques_.emissive_light->use();

	rendering_tool_.render_fullscreen_effect();
}

void Deferred_lighting_renderer::render_irradiance_volumes(const scene::Scene& scene, const Rendering_context& context)
{
	Rendering_device& device = rendering_tool_.device();

	device.set_input_layout(volume_input_layout_);

	for (auto v : scene.irradiance_volumes())
	{
		render_irradiance_volume(*v, context);
	}
}

void Deferred_lighting_renderer::render_irradiance_volume(const scene::Irradiance_volume& volume, const Rendering_context& context)
{
	const float4x4& world = volume.world_transformation();

	OBB obb(world);

	const auto& camera = context.camera();

	if (Intersection_type::Outside == camera.frustum().intersect(obb))
	{
		return;
	}

	Rendering_device& device = rendering_tool_.device();

	auto& change_per_light_data = change_per_light_.data();
	change_per_light_data.world = world;
	change_per_light_data.light_data = invert(world * camera.view());
	change_per_light_.update(device);

	device.set_shader_resources(scene::Irradiance_volume::get_num_textures(), volume.textures(), irradiance_volume_texture_offset_);

	techniques_.irradiance_volume->use();

	if (Intersection_type::Outside != Sphere(camera.world_position(), camera.greatest_distance_to_near_plane()).intersect(obb))
	{
		// camera is inside the light volume

		device.set_rasterizer_state(lighting_rasterizer_states_[Cull_state::Front]);
		device.set_depth_stencil_state(inside_lighting_volume_ds_state_light_, 1);
	}
	else
	{
		// camera is outside the light volume

		device.set_rasterizer_state(lighting_rasterizer_states_[Cull_state::Back]);
		device.set_depth_stencil_state(outside_lighting_volume_ds_state_prepare_, 1);
		device.set_blend_state(z_only_blend_state_);

		box_volume_.render(rendering_tool_);

		device.set_rasterizer_state(lighting_rasterizer_states_[Cull_state::Front]);
		device.set_depth_stencil_state(outside_lighting_volume_ds_state_light_, 2);
	}

	device.set_blend_state(lighting_blend_state_);

	box_volume_.render(rendering_tool_);
}

void Deferred_lighting_renderer::render_light_probes(const scene::Scene& scene, const Rendering_context& context)
{
	Rendering_device& device = rendering_tool_.device();

	device.set_input_layout(input_layout_);

	device.set_shader_resources(1, &scene::Light_probe::integrated_brdf(), light_2D_texture_offset0_);

	if (scene.surrounding_light_probe())
	{
		render_surrounding_light_probe(*scene.surrounding_light_probe(), context);
	}

	device.set_input_layout(volume_input_layout_);

	for (auto p : scene.light_probes())
	{
		render_light_probe(*p, context);
	}
}

void Deferred_lighting_renderer::render_surrounding_light_probe(const scene::Light_probe& light_probe, const Rendering_context& /*context*/)
{
	Rendering_device& device = rendering_tool_.device();

	device.set_rasterizer_state(rasterizer_state_cull_back_);
	device.set_depth_stencil_state(lighting_ds_state_, 1);
	device.set_blend_state(lighting_blend_state_);

	device.set_shader_resources(1, &light_probe.texture(), light_probe_texture_offset_);

	techniques_.light_probe_specular->use();

	rendering_tool_.render_fullscreen_effect();
}

void Deferred_lighting_renderer::render_light_probe(const scene::Light_probe& light_probe, const Rendering_context& context)
{
	const float4x4& world = light_probe.world_transformation();

	OBB obb(world);

	const auto& camera = context.camera();

	if (Intersection_type::Outside == camera.frustum().intersect(obb))
	{
		return;
	}

	Rendering_device& device = rendering_tool_.device();

	auto& change_per_light_data = change_per_light_.data();
	change_per_light_data.world = world;
	change_per_light_data.light_data = invert(world * camera.view());
	change_per_light_data.position_vs = light_probe.world_position() * camera.view();
	change_per_light_.update(device);

	device.set_shader_resources(1, &light_probe.texture(), light_probe_texture_offset_);

	techniques_.volume_light_probe_specular->use();

	if (Intersection_type::Outside != Sphere(camera.world_position(), camera.greatest_distance_to_near_plane()).intersect(obb))
	{
		// camera is inside the light volume

		device.set_rasterizer_state(lighting_rasterizer_states_[Cull_state::Front]);
		device.set_depth_stencil_state(inside_lighting_volume_ds_state_light_, 1);
	}
	else
	{
		// camera is outside the light volume

		device.set_rasterizer_state(lighting_rasterizer_states_[Cull_state::Back]);
		device.set_depth_stencil_state(outside_lighting_volume_ds_state_prepare_, 1);
		device.set_blend_state(z_only_blend_state_);

		box_volume_.render(rendering_tool_);

		device.set_rasterizer_state(lighting_rasterizer_states_[Cull_state::Front]);
		device.set_depth_stencil_state(outside_lighting_volume_ds_state_light_, 2);
	}

	device.set_blend_state(lighting_blend_state_);

	box_volume_.render(rendering_tool_);
}

void Deferred_lighting_renderer::render_directional_light(const scene::Light& light, const scene::Scene& scene, const Rendering_context& context)
{
	Rendering_device& device = rendering_tool_.device();

	if (light.casts_shadow())
	{
		directional_shadow_renderer_.render(light, scene, context);

		device.set_shader_resources(1, &directional_shadow_renderer_.white_buffer(), light_2D_texture_offset1_);
	}

	device.set_framebuffer(context.framebuffer());

	device.set_input_layout(input_layout_);

	device.set_rasterizer_state(rasterizer_state_cull_back_);
	device.set_depth_stencil_state(lighting_ds_state_, 1);
	device.set_blend_state(lighting_blend_state_);

	device.set_shader_resources(4, deferred_textures_);

	const auto& camera = context.camera();

	effect_->use(device);

	auto& change_per_light_data = change_per_light_.data();
	change_per_light_data.energy_and_range = float4(light.directional_energy(), 0.f);
	change_per_light_data.direction_vs = (float4(-light.world_direction(), 0.f) * camera.view()).xyz;
	change_per_light_.update(device);

	light.casts_shadow() ? techniques_.directional_light_with_shadow->use() : techniques_.directional_light->use();

	rendering_tool_.render_fullscreen_effect();
}

void Deferred_lighting_renderer::render_point_light(const scene::Light& light, const scene::Scene& /*scene*/, const Rendering_context& context)
{
	float4 energy_and_range = light.point_energy_and_range();
	float range = energy_and_range.w;

	Sphere sphere(light.world_position(), range);

	const auto& camera = context.camera();

	if (Intersection_type::Outside == camera.frustum().intersect(sphere))
	{
		return;
	}

	Rendering_device& device = rendering_tool_.device();

	device.set_framebuffer(context.framebuffer());

	device.set_input_layout(volume_input_layout_);

	device.set_shader_resources(4, deferred_textures_);

	effect_->use(device);

	float4x4 world;
	set_scale(world, range, range, range);
	set_origin(world, light.world_position());

	auto& change_per_light_data = change_per_light_.data();
	change_per_light_data.world = world;
	change_per_light_data.energy_and_range = energy_and_range;
	change_per_light_data.position_vs = light.world_position() * camera.view();
	change_per_light_.update(device);

	techniques_.point_light->use();

	AABB light_AABB;
	light_AABB.position = light.world_position();
	light_AABB.halfsize = float3(range, range, range);

	if (Intersection_type::Outside != Sphere(camera.world_position(), camera.greatest_distance_to_near_plane()).intersect(light_AABB))
	{
		// camera is inside the light volume

		device.set_rasterizer_state(lighting_rasterizer_states_[Cull_state::Front]);
		device.set_depth_stencil_state(inside_lighting_volume_ds_state_light_, 1);
	}
	else
	{
		// camera is outside the light volume

		device.set_rasterizer_state(lighting_rasterizer_states_[Cull_state::Back]);
		device.set_depth_stencil_state(outside_lighting_volume_ds_state_prepare_, 1);
		device.set_blend_state(z_only_blend_state_);

		sphere_volume_.render(rendering_tool_);

		device.set_rasterizer_state(lighting_rasterizer_states_[Cull_state::Front]);
		device.set_depth_stencil_state(outside_lighting_volume_ds_state_light_, 2);
	}

	device.set_blend_state(lighting_blend_state_);

	sphere_volume_.render(rendering_tool_);
}

void Deferred_lighting_renderer::render_spot_light(const scene::Light& light, const scene::Scene& scene, const Rendering_context& context)
{
	float4x4 view_projection = light.calculate_view_projection();

	Frustum light_frustum(view_projection);

	const auto& camera = context.camera();

	if (Intersection_type::Outside == camera.frustum().intersect(light_frustum))
	{
		return;
	}

	Rendering_device& device = rendering_tool_.device();

	auto& change_per_light_data = change_per_light_.data();

	if (light.casts_shadow())
	{
		spot_shadow_renderer_.render(light, light_frustum, scene, context);

		device.set_shader_resources(1, &spot_shadow_renderer_.shadow_map(), light_2D_texture_offset1_);

		change_per_light_data.light_data = invert(camera.view()) * spot_shadow_renderer_.view_projection() * scene::Light::texture_transform();
		change_per_light_data.shadow_linear_depth_projection = spot_shadow_renderer_.linear_depth_projection();
	}
	else
	{
		change_per_light_data.light_data = invert(camera.view()) * view_projection * scene::Light::texture_transform();
	}

	device.set_framebuffer(context.framebuffer());

	device.set_input_layout(volume_input_layout_);

	device.set_shader_resources(4, deferred_textures_);

	effect_->use(device);

	change_per_light_data.world = float4x4::identity;
	change_per_light_data.energy_and_range = light.spot_energy_and_range();
	change_per_light_data.position_vs = light.world_position() * camera.view();

	change_per_light_.update(device);

	device.set_shader_resources(1, &light.texture(), light_2D_texture_offset0_);

	light.casts_shadow() ? techniques_.spot_light_with_shadow->use() : techniques_.spot_light->use();

	frustum_volume_.setup(light.world_position(), light_frustum, rendering_tool_);

	if (Intersection_type::Outside != light_frustum.intersect(Sphere(camera.world_position(), camera.greatest_distance_to_near_plane())))
	{
		// camera is inside the light volume

		device.set_rasterizer_state(lighting_rasterizer_states_[Cull_state::Front]);
		device.set_depth_stencil_state(inside_lighting_volume_ds_state_light_, 1);
	}
	else
	{
		// camera is outside the light volume

	//	device.set_render_target(nullptr, context.m_depth_stencil_view);

		device.set_rasterizer_state(lighting_rasterizer_states_[Cull_state::Back]);
		device.set_depth_stencil_state(outside_lighting_volume_ds_state_prepare_, 1);
		device.set_blend_state(z_only_blend_state_);

		frustum_volume_.render(rendering_tool_);

		device.set_rasterizer_state(lighting_rasterizer_states_[Cull_state::Front]);
		device.set_depth_stencil_state(outside_lighting_volume_ds_state_light_, 2);
	}

	device.set_blend_state(lighting_blend_state_);

	frustum_volume_.render(rendering_tool_);
}

bool Deferred_lighting_renderer::create_render_states()
{
	auto& cache = rendering_tool_.render_state_cache();

	Rasterizer_state::Description rasterizer_description;
	rasterizer_description.cull_mode = Rasterizer_state::Description::Cull_mode::Back;

	rasterizer_state_cull_back_ = cache.rasterizer_state(rasterizer_description);
	if (!rasterizer_state_cull_back_)
	{
		return false;
	}

	rasterizer_description.cull_mode = Rasterizer_state::Description::Cull_mode::Front;

	rasterizer_state_cull_front_ = cache.rasterizer_state(rasterizer_description);
	if (!rasterizer_state_cull_front_)
	{
		return false;
	}

	Depth_stencil_state::Description ds_description;
	ds_description.depth_enable = false;
	ds_description.depth_write_mask = false;
	ds_description.comparison_func = Depth_stencil_state::Description::Comparison::Less;
	ds_description.stencil_enable = true;
	ds_description.front_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.comparison_func = Depth_stencil_state::Description::Comparison::Equal;
	ds_description.back_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.comparison_func = Depth_stencil_state::Description::Comparison::Equal;

	lighting_ds_state_ = cache.depth_stencil_state(ds_description);
	if (!lighting_ds_state_)
	{
		return false;
	}

	ds_description.depth_enable = true;
	ds_description.depth_write_mask = false;
	ds_description.comparison_func = Depth_stencil_state::Description::Comparison::Less_equal;
	ds_description.stencil_enable = true;
	ds_description.front_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Increment;
	ds_description.front_face.comparison_func = Depth_stencil_state::Description::Comparison::Equal;
	ds_description.back_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Increment;
	ds_description.back_face.comparison_func = Depth_stencil_state::Description::Comparison::Equal;

	outside_lighting_volume_ds_state_prepare_ = cache.depth_stencil_state(ds_description);
	if (!outside_lighting_volume_ds_state_prepare_)
	{
		return false;
	}

	ds_description.depth_enable = true;
	ds_description.depth_write_mask = false;
	ds_description.comparison_func = Depth_stencil_state::Description::Comparison::Greater_equal;
	ds_description.stencil_enable = true;
	ds_description.front_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Decrement;
	ds_description.front_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Decrement;
	ds_description.front_face.comparison_func = Depth_stencil_state::Description::Comparison::Equal;
	ds_description.back_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Decrement;
	ds_description.back_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Decrement;
	ds_description.back_face.comparison_func = Depth_stencil_state::Description::Comparison::Equal;

	outside_lighting_volume_ds_state_light_ = cache.depth_stencil_state(ds_description);
	if (!outside_lighting_volume_ds_state_light_)
	{
		return false;
	}

	ds_description.depth_enable = true;
	ds_description.depth_write_mask = false;
	ds_description.comparison_func = Depth_stencil_state::Description::Comparison::Greater_equal;
	ds_description.stencil_enable = true;
	ds_description.front_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.comparison_func = Depth_stencil_state::Description::Comparison::Equal;
	ds_description.back_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.comparison_func = Depth_stencil_state::Description::Comparison::Equal;

	inside_lighting_volume_ds_state_light_ = cache.depth_stencil_state(ds_description);
	if (!inside_lighting_volume_ds_state_light_)
	{
		return false;
	}

	Blend_state::Description blend_description;
	blend_description.independent_blend_enable = false;
	blend_description.render_targets[0].blend_enable            = true;
	blend_description.render_targets[0].source_blend            = Blend_state::Description::Blend::One;
	blend_description.render_targets[0].destination_blend       = Blend_state::Description::Blend::One;
	blend_description.render_targets[0].blend_op                = Blend_state::Description::Blend_op::Add;
	blend_description.render_targets[0].source_blend_alpha      = Blend_state::Description::Blend::Zero;
	blend_description.render_targets[0].destination_blend_alpha = Blend_state::Description::Blend::Zero;
	blend_description.render_targets[0].blend_op_alpha          = Blend_state::Description::Blend_op::Add;
	blend_description.render_targets[0].color_write_mask        = Blend_state::Description::Color_write_mask::Red | Blend_state::Description::Color_write_mask::Green | Blend_state::Description::Color_write_mask::Blue;

	lighting_blend_state_ = cache.blend_state(blend_description);
	if (!lighting_blend_state_)
	{
		return false;
	}

	blend_description.independent_blend_enable = false;
	blend_description.render_targets[0].blend_enable     = false;
	blend_description.render_targets[0].color_write_mask = 0;

	z_only_blend_state_ = cache.blend_state(blend_description);
	if (!z_only_blend_state_)
	{
		return false;
	}

	return true;
}

}
