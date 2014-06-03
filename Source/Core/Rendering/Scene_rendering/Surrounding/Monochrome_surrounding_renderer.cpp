#include "Monochrome_surrounding_renderer.hpp"
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

Monochrome_surrounding_renderer::Monochrome_surrounding_renderer(Rendering_tool& rendering_tool) : Surrounding_renderer(rendering_tool)
{}

Monochrome_surrounding_renderer::~Monochrome_surrounding_renderer()
{}

bool Monochrome_surrounding_renderer::init(Resource_manager& resource_manager)
{
	effect_ = resource_manager.load<Effect>("Effects/Surrounding/Monochrome_surrounding.effect");
	if (!effect_)
	{
		return false;
	}

	input_layout_ = rendering_tool_.vertex_layout_cache().input_layout(*Vertex_position2x32_tex_coord2x32::vertex_layout_description(), effect_->technique(0)->program()->signature());
	if (!input_layout_)
	{
		return false;
	}

	if (!color_buffer_.init(effect_, "Change_per_color"))
	{
		return false;
	}

	return create_render_states();
}

void Monochrome_surrounding_renderer::render(const scene::Scene& scene, const Rendering_context& context)
{
	clear(scene.surrounding().color(), context);
}

void Monochrome_surrounding_renderer::clear(const Color3& color, const Rendering_context& context)
{
	auto& device = rendering_tool_.device();

	device.set_viewports(1, &context.viewport());
	device.set_framebuffer(context.framebuffer());

	device.set_depth_stencil_state(ds_state_);
	device.set_blend_state(blend_state_);

	device.set_input_layout(input_layout_);

	effect_->use(device);

	color_buffer_.data().surrounding_color = color;
	color_buffer_.update(device);

	effect_->technique(0)->use();

	rendering_tool_.render_fullscreen_effect();
}

bool Monochrome_surrounding_renderer::create_render_states()
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

	ds_state_ = rendering_tool_.render_state_cache().depth_stencil_state(ds_description);
	if (!ds_state_)
	{
		return false;
	}

	Blend_state::Description blend_description;
	blend_description.render_targets[0].blend_enable     = false;
	blend_description.render_targets[0].color_write_mask = Blend_state::Description::Color_write_mask::Red | Blend_state::Description::Color_write_mask::Green | Blend_state::Description::Color_write_mask::Blue;

	blend_state_ = rendering_tool_.render_state_cache().blend_state(blend_description);
	if (!blend_state_)
	{
		return false;
	}

	return true;
}

}
