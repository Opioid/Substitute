#include "Resource_view.hpp"
#include "Texture.hpp"
#include <GL/glew.h>

namespace rendering
{

Texture_view::Texture_view(uint32_t id, const Texture_description& description, const Handle<Texture>& texture) :
	OpenGL_object(id),
	description_(description),
	internal_type_(Texture::gl_type(description.type)),
	texture_(texture)
{}

Texture_view::~Texture_view()
{
	glDeleteTextures(1, &id_);
}

uint32_t Texture_view::internal_type() const
{
	return internal_type_;
}

const Texture_description& Texture_view::description() const
{
	return description_;
}

const Handle<Texture>& Texture_view::texture() const
{
	return texture_;
}

Shader_resource_view::Shader_resource_view(uint32_t id, const Texture_description& description, const Handle<Texture>& texture, const std::string& name) :
	Texture_view(id, description, texture), Manageable(name)
{}

size_t Shader_resource_view::num_bytes() const
{
	uint32_t elements_per_block = Data_format::num_elements_per_block(description_.format);

	uint32_t num_mip_levels = description_.num_mip_levels;

	uint3 dimensions = description_.dimensions;

	uint32_t num_blocks = 0;

	while (num_mip_levels > 0)
	{
		num_blocks += (dimensions.x * dimensions.y) / elements_per_block * dimensions.z;

		dimensions.x = std::max(dimensions.x / 2, uint32_t(1));
		dimensions.y = std::max(dimensions.y / 2, uint32_t(1));
		dimensions.z = std::max(dimensions.z / 2, uint32_t(1));

		--num_mip_levels;
	}

	return static_cast<size_t>(description_.num_layers * num_blocks * Data_format::num_bytes_per_block(description_.format));
}

Render_target_view::Render_target_view(uint32_t id, const Texture_description& description, const Handle<Texture>& texture) :
	Texture_view(id, description, texture)
{}

Render_target_shader_resource_view::Render_target_shader_resource_view(const Handle<Render_target_view>& Render_target_view, const Handle<Shader_resource_view>& shader_resource_view) :
	render_target_view_(Render_target_view), shader_resource_view_(shader_resource_view)
{}

const Handle<Render_target_view>& Render_target_shader_resource_view::render_target_view() const
{
	return render_target_view_;
}

Handle<Render_target_view>& Render_target_shader_resource_view::render_target_view()
{
	return render_target_view_;
}

const Handle<Shader_resource_view>& Render_target_shader_resource_view::shader_resource_view() const
{
	return shader_resource_view_;
}

Handle<Shader_resource_view>& Render_target_shader_resource_view::shader_resource_view()
{
	return shader_resource_view_;
}

Cube_render_target_shader_resource_view::Cube_render_target_shader_resource_view(const Handle<Render_target_view>* Render_target_views, const Handle<Shader_resource_view>& shader_resource_view) :
	shader_resource_view_(shader_resource_view)
{
	Render_target_views_[0] = Render_target_views[0];
	Render_target_views_[1] = Render_target_views[1];
	Render_target_views_[2] = Render_target_views[2];
	Render_target_views_[3] = Render_target_views[3];
	Render_target_views_[4] = Render_target_views[4];
	Render_target_views_[5] = Render_target_views[5];
}

const Handle<Render_target_view>& Cube_render_target_shader_resource_view::render_target_view(uint32_t face) const
{
	return Render_target_views_[face];
}

Handle<Render_target_view>& Cube_render_target_shader_resource_view::render_target_view(uint32_t face)
{
	return Render_target_views_[face];
}

const Handle<Shader_resource_view>& Cube_render_target_shader_resource_view::shader_resource_view() const
{
	return shader_resource_view_;
}

Handle<Shader_resource_view>& Cube_render_target_shader_resource_view::shader_resource_view()
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

const Handle<Depth_stencil_view>& Depth_stencil_shader_resource_view::depth_stencil_view() const
{
	return depth_stencil_view_;
}

Handle<Depth_stencil_view>& Depth_stencil_shader_resource_view::depth_stencil_view()
{
	return depth_stencil_view_;
}

const Handle<Shader_resource_view>& Depth_stencil_shader_resource_view::shader_resource_view() const
{
	return shader_resource_view_;
}

Handle<Shader_resource_view>& Depth_stencil_shader_resource_view::shader_resource_view()
{
	return shader_resource_view_;
}

}
