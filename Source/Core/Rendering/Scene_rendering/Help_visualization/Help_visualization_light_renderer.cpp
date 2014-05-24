#include "Help_visualization_light_renderer.hpp"
#include "Rendering/Rendering_tool.hpp"
#include "Rendering/Vertex_format.hpp"
#include "Rendering/Input_layout.hpp"
#include "Rendering/Rendering_context.hpp"
#include "Rendering/Effect/Effect.hpp"
#include "Resources/Resource_manager.hpp"
#include "Math/Intersection/Sphere.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Model.hpp"
#include "Scene/Light/Light.hpp"
#include "Scene/Light/Irradiance_volume.hpp"

namespace rendering
{

Help_visualization_light_renderer::Help_visualization_light_renderer(Rendering_tool& rendering_tool) : Scene_renderer(rendering_tool)
{

}

Help_visualization_light_renderer::~Help_visualization_light_renderer()
{
}

bool Help_visualization_light_renderer::init(Resource_manager& resource_manager, Constant_buffer_cache& /*constant_buffer_cache*/)
{
	sphere_model_ = resource_manager.load<scene::Model>("Models/Sphere.sum");
	if (!sphere_model_)
	{
		return false;
	}

	effect_ = resource_manager.load<Effect>("Effects/Help_visualization/Help_visualization_light_renderer.effect");
	if (!effect_)
	{
		return false;
	}

	light_probe_technique_  = effect_->technique("Light_probe");

	input_layout_ = rendering_tool_.vertex_layout_cache().input_layout(*Vertex_position3x32_tex_coord2x32_normal1010102::vertex_layout_description(), light_probe_technique_->program()->signature());
	if (!input_layout_)
	{
		return false;
	}

	if (!change_per_camera_.init(effect_, "Change_per_camera"))
	{
		return false;
	}

	if (!change_per_light_.init(effect_, "Change_per_light"))
	{
		return false;
	}

	if (!change_per_object_.init(effect_, "Change_per_object"))
	{
		return false;
	}

	return create_render_states();
}

bool Help_visualization_light_renderer::on_resize_targets(const uint2& /*size*/, const Handle<Depth_stencil_shader_resource_view>& /*depth_stencil*/)
{
	return true;
}

void Help_visualization_light_renderer::render(const scene::Scene& scene, const Rendering_context& context)
{
	auto& device = rendering_tool_.device();

	device.set_framebuffer(context.framebuffer());
	device.clear_depth_stencil(context.framebuffer(), 1.f, 0);

	device.set_rasterizer_state(rasterizer_state_);
	device.set_depth_stencil_state(ds_state_);
	device.set_blend_state(blend_state_);

	device.set_primitive_topology(Primitive_topology::Triangle_list);
	device.set_input_layout(input_layout_);

	device.set_vertex_buffers(sphere_model_->vertex_layout_description()->num_streams(),
							  sphere_model_->vertex_buffers(),
							  sphere_model_->vertex_layout_description()->strides());

	device.set_index_buffer(sphere_model_->index_buffer());

	effect_->use(device);
	light_probe_technique_->use();

	const auto& camera = context.camera();

	change_per_camera_.data().view_projection = camera.view_projection();
	change_per_camera_.update(device);

	auto& volumes = scene.get_irradiance_volumes();

	for (auto volume : volumes)
	{
		render(*volume, camera);
	}
}

void Help_visualization_light_renderer::render(const scene::Irradiance_volume& volume, const scene::Camera& camera)
{
	auto& device = rendering_tool_.device();

	const float4x4& world = volume.world_transformation();

	change_per_light_.data().light_data = invert(camera.view()) * invert(world);
	change_per_light_.update(device);

	device.set_shader_resources(scene::Irradiance_volume::get_num_textures(), volume.textures());

	uint32_t num_probes = volume.get_num_probes();
	for (uint32_t i = 0; i < num_probes; ++i)
	{
		render_diffuse_probe(volume.get_probe_position(i), camera.view());
	}
}

void Help_visualization_light_renderer::render_diffuse_probe(const float3& position, const float4x4& view)
{
	auto& device = rendering_tool_.device();

	float4x4 world = float4x4::identity;
	set_scale(world, float3(0.25f, 0.25f, 0.25f));
	set_origin(world, position);

	auto& change_per_object_data = change_per_object_.data();
	change_per_object_data.world = world;
	change_per_object_data.position_vs = position * view;
	change_per_object_.update(device);

	device.draw_indexed(sphere_model_->num_indices());
}

bool Help_visualization_light_renderer::create_render_states()
{
	Rasterizer_state::Description rasterizer_description;
	rasterizer_description.cull_mode = Rasterizer_state::Description::Cull_mode::Back;
	rasterizer_state_ = rendering_tool_.render_state_cache().get_rasterizer_state(rasterizer_description);
	if (!rasterizer_state_)
	{
		return false;
	}

	Depth_stencil_state::Description ds_description;
	ds_description.depth_enable = true;
	ds_description.depth_write_mask = true;
	ds_description.comparison_func = Depth_stencil_state::Description::Comparison::Less;

	ds_state_ = rendering_tool_.render_state_cache().get_depth_stencil_state(ds_description);
	if (!ds_state_)
	{
		return false;
	}

	Blend_state::Description blend_description;
	blend_description.independent_blend_enable = false;
	blend_description.render_targets[0].blend_enable     = false;
	blend_description.render_targets[0].color_write_mask = Blend_state::Description::Color_write_mask::Red | Blend_state::Description::Color_write_mask::Green | Blend_state::Description::Color_write_mask::Blue;

	blend_state_ = rendering_tool_.render_state_cache().get_blend_state(blend_description);
	if (!blend_state_)
	{
		return false;
	}

	return true;
}

}
