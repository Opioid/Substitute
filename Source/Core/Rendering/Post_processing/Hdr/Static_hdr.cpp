#include "Static_hdr.hpp"
#include "Rendering/Rendering_tool.hpp"
#include "Rendering/Input_layout.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Vertex_format.hpp"
#include "Rendering/Rendering_context.hpp"
#include "Rendering/Effect/Effect.hpp"
#include "Resources/Resource_manager.hpp"
#include "Logging/Logging.hpp"

namespace rendering
{

Static_hdr::Static_hdr(Rendering_tool& rendering_tool, Data_format::Value hdr_format) : Hdr_filter(rendering_tool, hdr_format)
{}

bool Static_hdr::init(Resource_manager& resource_manager)
{
	effect_ = resource_manager.load<Effect>("Effects/Hdr/Static_hdr.effect");
	if (!effect_)
	{
		return false;
	}

	compose_technique_ = effect_->technique("Compose");

	input_layout_ = rendering_tool_.vertex_layout_cache().input_layout(*Vertex_position2x32::vertex_layout_description(), effect_->technique(0)->program()->signature());
	if (!input_layout_)
	{
		return false;
	}

	if (!change_per_setting_.init(effect_, "Change_per_setting"))
	{
		return false;
	}

	return create_render_states();
}

void Static_hdr::render(const Handle<Shader_resource_view>& source, const Viewport& /*source_viewport*/, const Rendering_context& context)
{
	Rendering_device& device = rendering_tool_.device();

	device.set_input_layout(input_layout_);

	device.set_shader_resources(1, &source);
	effect_->use(device);

	device.set_depth_stencil_state(ds_state_);

	device.set_framebuffer(context.framebuffer());
	device.set_viewports(1, &context.viewport());

	device.set_blend_state(compose_blend_state_);

	change_per_setting_.data().linear_white_and_exposure = float4(linear_white_, exposure_);
	change_per_setting_.update(device);

	compose_technique_->use();

	rendering_tool_.render_fullscreen_effect();
}

bool Static_hdr::on_resize_targets(const uint2& /*dimensions*/)
{
	return true;
}

bool Static_hdr::create_render_states()
{
	Depth_stencil_state::Description ds_description;
	ds_description.depth_enable = false;
	ds_description.depth_write_mask = false;
	ds_description.stencil_enable = false;

	ds_state_ = rendering_tool_.render_state_cache().get_depth_stencil_state(ds_description);
	if (!ds_state_)
	{
		return false;
	}

	Blend_state::Description blend_description;
	blend_description.independent_blend_enable = false;
	blend_description.render_targets[0].blend_enable     = false;
	blend_description.render_targets[0].color_write_mask = Blend_state::Description::Color_write_mask::All;

	compose_blend_state_ = rendering_tool_.render_state_cache().get_blend_state(blend_description);
	if (!compose_blend_state_)
	{
		return false;
	}

	return true;
}

}
