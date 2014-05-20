#include "Fxaa.hpp"
#include "Rendering/Rendering_tool.hpp"
#include "Rendering/Input_layout.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Vertex_format.hpp"
#include "Rendering/Rendering_context.hpp"
#include "Rendering/Effect/Effect.hpp"
#include "Resources/Resource_manager.hpp"

namespace rendering
{

Fxaa::Fxaa(Rendering_tool& rendering_tool) : Post_processing_effect(rendering_tool)
{}

bool Fxaa::init(Resource_manager& resource_manager)
{
	effect_ = resource_manager.load<Effect>("Effects/Fxaa/Fxaa.effect");
	if (!effect_)
	{
		return false;
	}

	input_layout_ = rendering_tool_.get_vertex_layout_cache().get_input_layout(*Vertex_position2x32_tex_coord2x32::vertex_layout_description(), effect_->get_technique(0)->get_program()->get_signature());
	if (!input_layout_)
	{
		return false;
	}

	if (!change_per_source_.init(effect_, "Change_per_source"))
	{
		return false;
	}

	return create_render_states();
}

void Fxaa::render(const Handle<Shader_resource_view>& source, const Viewport& source_viewport, const Rendering_context& context)
{
	Rendering_device& device = rendering_tool_.get_device();

	device.set_framebuffer(context.get_framebuffer());
	device.set_viewports(1, &context.get_viewport());

	device.set_depth_stencil_state(ds_state_);
	device.set_blend_state(blend_state_);

	device.set_input_layout(input_layout_);

	device.set_shader_resources(1, &source);

	effect_->use(device);

	change_per_source_.get_data().inverse_source_size = reciprocal(source_viewport.size);
	change_per_source_.update(device);

	effect_->get_technique(0)->use();

	rendering_tool_.render_fullscreen_effect();
}

bool Fxaa::on_resize_targets(const uint2& /*size*/)
{
	return true;
}

bool Fxaa::create_render_states()
{
	Depth_stencil_state::Description ds_description;
	ds_description.depth_enable = false;
	ds_description.depth_write_mask = false;
	ds_description.stencil_enable = false;

	ds_state_ = rendering_tool_.get_render_state_cache().get_depth_stencil_state(ds_description);
	if (!ds_state_)
	{
		return false;
	}

	Blend_state::Description blend_description;
	blend_description.independent_blend_enable = false;
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
