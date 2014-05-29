#include "Post_processing_pipeline.hpp"
#include "Post_processing_effect.hpp"
#include "Rendering/Rendering_tool.hpp"
#include "Rendering/Resource_view.hpp"

namespace rendering
{

Post_processing_pipeline::Post_processing_pipeline(Rendering_tool& rendering_tool) : Rendering_object(rendering_tool), m_hdr_effect(nullptr), effect_(nullptr)
{}

bool Post_processing_pipeline::init(Resource_manager& /*resource_manager*/)
{
	Rendering_device& device = rendering_tool_.device();

	framebuffer_ = device.create_framebuffer();

	if (!framebuffer_)
	{
		return false;
	}

	return true;
}

bool Post_processing_pipeline::resize_targets(const uint2& size, Data_format::Value format)
{
	Rendering_device& device = rendering_tool_.device();

	Texture_description texture_description;
	texture_description.type = Texture_description::Type::Texture_2D;
	texture_description.dimensions.xy = size;
	texture_description.format = format;
	m_scratch_texture = device.create_render_target_shader_resource_view(texture_description);

	if (!m_scratch_texture)
	{
		return false;
	}

	framebuffer_->set_render_targets(m_scratch_texture->render_target_view());

	if (!framebuffer_->is_valid())
	{
		return false;
	}

	return true;
}

void Post_processing_pipeline::render(const Handle<Shader_resource_view>& source, const Viewport& source_viewport, const Rendering_context& context)
{
	context_.set_camera(context.camera());

	if (!effect_)
	{
		context_.set_viewport(context.viewport());
		context_.set_framebuffer(context.framebuffer());
	}
	else
	{
		context_.set_viewport(source_viewport);
		context_.set_framebuffer(framebuffer_);
	}

	if (m_hdr_effect)
	{
		m_hdr_effect->render(source, source_viewport, context_);
	}

	if (effect_)
	{
		effect_->render(m_scratch_texture->shader_resource_view(), source_viewport, context);
	}

}

void Post_processing_pipeline::set_hdr_effect(Post_processing_effect* hdr_effect)
{
	m_hdr_effect = hdr_effect;
}

void Post_processing_pipeline::push_back_effect(Post_processing_effect* effect)
{
	effect_ = effect;
}

}
