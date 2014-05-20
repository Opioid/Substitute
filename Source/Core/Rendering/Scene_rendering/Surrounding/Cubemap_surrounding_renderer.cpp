#include "Cubemap_surrounding_renderer.hpp"
#include "Rendering/Rendering_tool.hpp"
#include "Rendering/Input_layout.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Vertex_format.hpp"
#include "Rendering/Rendering_context.hpp"
#include "Rendering/Effect/Effect.hpp"
#include "Resources/Resource_manager.hpp"
#include "Scene/Scene.hpp"

namespace rendering
{

Cubemap_surrounding_renderer::Cubemap_surrounding_renderer(Rendering_tool& rendering_tool) : Surrounding_renderer(rendering_tool)
{}

Cubemap_surrounding_renderer::~Cubemap_surrounding_renderer()
{}

bool Cubemap_surrounding_renderer::init(Resource_manager& resource_manager)
{
	effect_ = resource_manager.load<Effect>("Effects/Surrounding/Cubemap_surrounding.effect");
	if (!effect_)
	{
		return false;
	}

	input_layout_ = rendering_tool_.get_vertex_layout_cache().get_input_layout(*Vertex_position2x32_tex_coord2x32::vertex_layout_description(), effect_->get_technique(0)->get_program()->get_signature());
	if (!input_layout_)
	{
		return false;
	}

	if (!change_per_camera_.init(effect_, "Change_per_camera"))
	{
		return false;
	}

	if (!clear_buffer_.init(effect_, "Change_per_color"))
	{
		return false;
	}

	return create_render_states();
}

void Cubemap_surrounding_renderer::render(const scene::Scene& scene, const Rendering_context& context)
{
	const Handle<Shader_resource_view>& cubemap = scene.get_surrounding().get_texture();

    if (!cubemap)
    {
        clear(scene.get_surrounding().get_color(), context);
        return;
    }

	Rendering_device& device = rendering_tool_.get_device();

	device.set_viewports(1, &context.get_viewport());
	device.set_framebuffer(context.get_framebuffer());

	device.set_depth_stencil_state(ds_state_);
	device.set_blend_state(blend_state_);

	device.set_input_layout(input_layout_);

	effect_->use(device);

	const scene::Camera& camera = context.get_camera();

	const float3* rays = camera.get_view_rays_ws();
	auto& change_per_camera_data = change_per_camera_.get_data();
	change_per_camera_data.rays[0] = float4(rays[0], 1.f);
	change_per_camera_data.rays[1] = float4(rays[1], 1.f);
	change_per_camera_data.rays[2] = float4(rays[2], 1.f);
	change_per_camera_.update(device);

	device.set_shader_resources(1, &cubemap);

	effect_->get_technique(0)->use();

	rendering_tool_.render_fullscreen_effect();
}

void Cubemap_surrounding_renderer::clear(const Color3& color, const Rendering_context& context)
{
	Rendering_device& device = rendering_tool_.get_device();

	device.set_viewports(1, &context.get_viewport());
	device.set_framebuffer(context.get_framebuffer());

	device.set_depth_stencil_state(ds_state_);
	device.set_blend_state(blend_state_);

	device.set_input_layout(input_layout_);

	effect_->use(device);

	clear_buffer_.get_data().clear_color = color;
	clear_buffer_.update(device);

	effect_->get_technique(1)->use();

	rendering_tool_.render_fullscreen_effect();
}

bool Cubemap_surrounding_renderer::create_render_states()
{
	Depth_stencil_state::Description ds_description;
	ds_description.depth_enable = false;
	ds_description.depth_write_mask = false;
	ds_description.stencil_enable = true;
	ds_description.front_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.front_face.comparison_func = Depth_stencil_state::Description::Comparison::Equal;
	ds_description.back_face.fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.depth_fail_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.pass_op = Depth_stencil_state::Description::Stencil::Stencil_op::Keep;
	ds_description.back_face.comparison_func = Depth_stencil_state::Description::Comparison::Equal;

	ds_state_ = rendering_tool_.get_render_state_cache().get_depth_stencil_state(ds_description);
	if (!ds_state_)
	{
		return false;
	}

	Blend_state::Description blend_description;
	blend_description.render_targets[0].blend_enable     = false;
	blend_description.render_targets[0].color_write_mask = Blend_state::Description::Color_write_mask::Red | Blend_state::Description::Color_write_mask::Green | Blend_state::Description::Color_write_mask::Blue;

	blend_state_ = rendering_tool_.get_render_state_cache().get_blend_state(blend_description);
	if (!blend_state_)
	{
		return false;
	}

	return true;
}

}