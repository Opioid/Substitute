#pragma once

#include "OpenGL_object.hpp"
#include "Resources/Resource.hpp"
#include "Resources/Manageable.hpp"
#include "Resources/Handle.hpp"
#include "Rendering/Texture_description.hpp"

namespace rendering
{

class Rendering_device;
class Framebuffer;
class Texture;

class Texture_view : public OpenGL_object
{

public:

	Texture_view(uint32_t id, const Texture_description& description, const Handle<Texture>& texture);
	~Texture_view();

	uint32_t get_internal_type() const;

	const Texture_description& get_description() const;

	const Handle<Texture>& get_texture() const;

private:

	Texture_description description_;
	uint32_t internal_type_;

	Handle<Texture> texture_;

	friend Rendering_device;
	friend Framebuffer;
};

class Shader_resource_view : public Texture_view, public Resource, public Manageable
{

public:

	Shader_resource_view(uint32_t id, const Texture_description& description, const Handle<Texture>& texture, const std::string& name = "");

private:

	friend Rendering_device;
};

class Render_target_view : public Texture_view
{

public:

	Render_target_view(uint32_t id, const Texture_description& description, const Handle<Texture>& texture);

private:

	friend Rendering_device;
	friend Framebuffer;
};

class Render_target_shader_resource_view : public Reference_counter
{

public:

	Render_target_shader_resource_view(const Handle<Render_target_view>& render_target_view, const Handle<Shader_resource_view>& shader_resource_view);

	const Handle<Render_target_view>& get_render_target_view() const;
	Handle<Render_target_view>& get_render_target_view();

	const Handle<Shader_resource_view>& get_shader_resource_view() const;
	Handle<Shader_resource_view>& get_shader_resource_view();

private:

	Handle<Render_target_view>   render_target_view_;
	Handle<Shader_resource_view> shader_resource_view_;

	friend Rendering_device;
};

class Cube_render_target_shader_resource_view : public Reference_counter
{

public:

	Cube_render_target_shader_resource_view(const Handle<Render_target_view>* render_target_views, const Handle<Shader_resource_view>& shader_resource_view);

	const Handle<Render_target_view>& get_render_target_view(uint32_t face) const;
	Handle<Render_target_view>& get_render_target_view(uint32_t face);

	const Handle<Shader_resource_view>& get_shader_resource_view() const;
	Handle<Shader_resource_view>& get_shader_resource_view();

private:

	Handle<Render_target_view>   render_target_views_[6];
	Handle<Shader_resource_view> shader_resource_view_;

	friend Rendering_device;
};

class Depth_stencil_view : public Texture_view
{

public:

	Depth_stencil_view(uint32_t id, const Texture_description& description, const Handle<Texture>& texture);
	virtual ~Depth_stencil_view();

private:

	friend Rendering_device;
	friend Framebuffer;
};

class Depth_stencil_shader_resource_view : public Reference_counter
{

public:

	Depth_stencil_shader_resource_view(const Handle<Depth_stencil_view>& depth_stencil_view, const Handle<Shader_resource_view>& shader_resource_view);

	const Handle<Depth_stencil_view>& get_depth_stencil_view() const;
	Handle<Depth_stencil_view>& get_depth_stencil_view();

	const Handle<Shader_resource_view>& get_shader_resource_view() const;
	Handle<Shader_resource_view>& get_shader_resource_view();

private:

	Handle<Depth_stencil_view>   depth_stencil_view_;
	Handle<Shader_resource_view> shader_resource_view_;

	friend Rendering_device;
};

}


