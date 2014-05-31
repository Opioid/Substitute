#include "Framebuffer.hpp"
#include "Resource_view.hpp"
#include "Texture.hpp"
#include <GL/glew.h>

namespace rendering
{

Framebuffer::Framebuffer(uint32_t id) : OpenGL_object(id)
{}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &id_);
}

bool Framebuffer::is_valid() const
{
	return GL_FRAMEBUFFER_COMPLETE == glCheckNamedFramebufferStatusEXT(id_, GL_FRAMEBUFFER);
}

void Framebuffer::set_render_targets(const Handle<Render_target_view>& target)
{
	set_draw_buffers(1);

	set_target(target, 0);
}

void Framebuffer::set_render_targets(const Handle<Depth_stencil_view>& depth_stencil)
{
	set_depth_stencil(depth_stencil);
}

void Framebuffer::set_render_targets(const Handle<Render_target_view>& target, const Handle<Depth_stencil_view>& depth_stencil)
{
	set_draw_buffers(1);

	set_target(target, 0);

	set_depth_stencil(depth_stencil);
}

void Framebuffer::set_render_targets(std::initializer_list<Handle<Render_target_view>> targets, const Handle<Depth_stencil_view>& depth_stencil)
{
	set_draw_buffers(static_cast<uint32_t>(targets.size()));

	uint32_t slot = 0;
	for (const auto& target : targets)
	{
		set_target(target, slot++);
	}

	set_depth_stencil(depth_stencil);
}

void Framebuffer::set_draw_buffers(uint32_t num_targets) const
{
	static const uint32_t attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 };

	glFramebufferDrawBuffersEXT(id_, num_targets, attachments);
}

void Framebuffer::set_target(const Handle<Render_target_view>& target, uint32_t slot) const
{
	glNamedFramebufferTextureEXT(id_, GL_COLOR_ATTACHMENT0 + slot, target->id_, 0);
}

void Framebuffer::set_depth_stencil(const Handle<Depth_stencil_view>& depth_stencil) const
{
	const auto& description = depth_stencil->description_;

	uint32_t attachment_type = get_attachment_type(description.format);

	if (description.shader_resource)
	{
		glNamedFramebufferTextureEXT(id_, attachment_type, depth_stencil->id_, 0);
	}
	else
	{
		glNamedFramebufferRenderbufferEXT(id_, attachment_type, GL_RENDERBUFFER_EXT, depth_stencil->id_);
	}
}

uint32_t Framebuffer::get_attachment_type(Data_format::Value format)
{
	switch (format)
	{
		case Data_format::D32_Float:
			return GL_DEPTH_ATTACHMENT;
		case Data_format::D24_UNorm_S8_UInt:
		case Data_format::D32_Float_S8X24_UInt:
			return GL_DEPTH_STENCIL_ATTACHMENT;
		default:
			return 0;
	}
}

}
