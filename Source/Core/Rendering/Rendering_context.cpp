#include "Rendering_context.hpp"
#include "Rendering/Framebuffer.hpp"
#include "../Scene/Camera.hpp"

namespace rendering
{

Rendering_context::Rendering_context() : camera_(nullptr)
{
	options_.set(Options::Render_actors, true);
	options_.set(Options::Render_static_geometry, true);
	options_.set(Options::Render_surrounding, true);
	options_.set(Options::Render_analytical_lighting, true);
	options_.set(Options::Render_image_based_lighting, true);
	options_.set(Options::Render_emissive_lighting, true);
	options_.set(Options::Face_culling, true);
}

const scene::Camera& Rendering_context::get_camera() const
{
	return *camera_;
}

void Rendering_context::set_camera(const scene::Camera& camera)
{
	camera_ = &camera;
}

const Viewport& Rendering_context::get_viewport() const
{
	return viewport_;
}

void Rendering_context::set_viewport(const Viewport& viewport)
{
	viewport_ = viewport;
}

const Handle<Framebuffer>& Rendering_context::get_framebuffer() const
{
	return framebuffer_;
}

Handle<Framebuffer>& Rendering_context::get_framebuffer()
{
	return framebuffer_;
}

void Rendering_context::set_framebuffer(const Handle<Framebuffer>& framebuffer)
{
	framebuffer_ = framebuffer;
}

const Rendering_context::Rendering_options& Rendering_context::get_options() const
{
	return options_;
}

Rendering_context::Rendering_options& Rendering_context::get_options()
{
	return options_;
}

void Rendering_context::set_options(const Rendering_options& options)
{
	options_ = options;
}

}
