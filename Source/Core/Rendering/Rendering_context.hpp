#pragma once

#include "Rendering/Viewport.hpp"
#include "Resources/Handle.hpp"
#include "Flags/Flags.hpp"

namespace scene
{

class Camera;

}

namespace rendering
{

class Framebuffer;

class Rendering_context
{

public:

	enum class Options
	{
		Render_actors				= 1 << 0,
		Render_static_geometry 	    = 1 << 1,
		Render_surrounding			= 1 << 2,
		Render_analytical_lighting  = 1 << 3,
		Render_image_based_lighting = 1 << 4,
		Render_emissive_lighting    = 1 << 5,
		Face_culling                = 1 << 6
	};

	typedef Flags<Options, uint8_t> Rendering_options;

	Rendering_context();

	const scene::Camera& camera() const;
	void set_camera(const scene::Camera& camera);

	const Viewport& viewport() const;
	void set_viewport(const Viewport& viewport);

	const Handle<Framebuffer>& framebuffer() const;
	Handle<Framebuffer>& framebuffer();
	void set_framebuffer(const Handle<Framebuffer>& framebuffer);

	const Rendering_options& options() const;
	Rendering_options& options();
	void set_options(const Rendering_options& options);

private:

	const scene::Camera* camera_;

	Viewport viewport_;

	Handle<Framebuffer> framebuffer_;

	Rendering_options options_;
};

}



