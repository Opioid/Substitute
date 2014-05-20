#include "Resource_view.hpp"
#include "Texture.hpp"
#include <GL/glew.h>

namespace rendering
{

Texture_view::Texture_view(uint32_t id, const Texture_description& description, const Handle<Texture>& texture) :
	OpenGL_object(id),
	description_(description),
	internal_type_(Texture::get_gl_type(description.type)),
	texture_(texture)
{}

Texture_view::~Texture_view()
{
	glDeleteTextures(1, &id_);
}

uint32_t Texture_view::get_internal_type() const
{
	return internal_type_;
}

const Texture_description& Texture_view::get_description() const
{
	return description_;
}

const Handle<Texture>& Texture_view::get_texture() const
{
	return texture_;
}

Shader_resource_view::Shader_resource_view(uint32_t id, const Texture_description& description, const Handle<Texture>& texture, const std::string& name) :
	Texture_view(id, description, texture), Manageable(name)
{}

Render_target_view::Render_target_view(uint32_t id, const Texture_description& description, const Handle<Texture>& texture) :
	Texture_view(id, description, texture)
{}

Render_target_shader_resource_view::Render_target_shader_resource_view(const Handle<Render_target_view>& render_target_view, const Handle<Shader_resource_view>& shader_resource_view) :
	render_target_view_(render_target_view), shader_resource_view_(shader_resource_view)
{}

const Handle<Render_target_view>& Render_target_shader_resource_view::get_render_target_view() const
{
	return render_target_view_;
}

Handle<Render_target_view>& Render_target_shader_resource_view::get_render_target_view()
{
	return render_target_view_;
}

const Handle<Shader_resource_view>& Render_target_shader_resource_view::get_shader_resource_view() const
{
	return shader_resource_view_;
}

Handle<Shader_resource_view>& Render_target_shader_resource_view::get_shader_resource_view()
{
	return shader_resource_view_;
}

Cube_render_target_shader_resource_view::Cube_render_target_shader_resource_view(const Handle<Render_target_view>* render_target_views, const Handle<Shader_resource_view>& shader_resource_view) :
	shader_resource_view_(shader_resource_view)
{
	render_target_views_[0] = render_target_views[0];
	render_target_views_[1] = render_target_views[1];
	render_target_views_[2] = render_target_views[2];
	render_target_views_[3] = render_target_views[3];
	render_target_views_[4] = render_target_views[4];
	render_target_views_[5] = render_target_views[5];
}

const Handle<Render_target_view>& Cube_render_target_shader_resource_view::get_render_target_view(uint32_t face) const
{
	return render_target_views_[face];
}

Handle<Render_target_view>& Cube_render_target_shader_resource_view::get_render_target_view(uint32_t face)
{
	return render_target_views_[face];
}

const Handle<Shader_resource_view>& Cube_render_target_shader_resource_view::get_shader_resource_view() const
{
	return shader_resource_view_;
}

Handle<Shader_resource_view>& Cube_render_target_shader_resource_view::get_shader_resource_view()
{
	return shader_resource_view_;
}

Depth_stencil_view::Depth_stencil_view(uint32_t id, const Texture_description& description, const Handle<Texture>& texture) : Texture_view(id, description, texture)
{}

Depth_stencil_view::~Depth_stencil_view()
{
	// Nothing to delete here, because Depth_stencil_view don't has its own resource
}

Depth_stencil_shader_resource_view::Depth_stencil_shader_resource_view(const Handle<Depth_stencil_view>& depth_stencil_view, const Handle<Shader_resource_view>& shader_resource_view) :
	depth_stencil_view_(depth_stencil_view), shader_resource_view_(shader_resource_view)
{}

const Handle<Depth_stencil_view>& Depth_stencil_shader_resource_view::get_depth_stencil_view() const
{
	return depth_stencil_view_;
}

Handle<Depth_stencil_view>& Depth_stencil_shader_resource_view::get_depth_stencil_view()
{
	return depth_stencil_view_;
}

const Handle<Shader_resource_view>& Depth_stencil_shader_resource_view::get_shader_resource_view() const
{
	return shader_resource_view_;
}

Handle<Shader_resource_view>& Depth_stencil_shader_resource_view::get_shader_resource_view()
{
	return shader_resource_view_;
}

}
