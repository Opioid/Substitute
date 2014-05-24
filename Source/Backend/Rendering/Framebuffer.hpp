#pragma once

#include "OpenGL_object.hpp"
#include "Rendering/Data_format.hpp"
#include "Resources/Handle.hpp"
#include <initializer_list>

namespace rendering
{

class Rendering_device;
class Render_tarview;
class Depth_stencil_view;

class Framebuffer : public OpenGL_object
{

public:

	Framebuffer(uint32_t id = 0);
	~Framebuffer();

	bool is_valid() const;

	void set_render_targets(const Handle<Render_tarview>& target);

	void set_render_targets(const Handle<Depth_stencil_view>& depth_stencil);

	void set_render_targets(const Handle<Render_tarview>& target, const Handle<Depth_stencil_view>& depth_stencil);

	void set_render_targets(std::initializer_list<Handle<Render_tarview>> targets, const Handle<Depth_stencil_view>& depth_stencil);

private:

	void set_draw_buffers(uint32_t num_targets) const;

	void set_target(const Handle<Render_tarview>& target, uint32_t slot) const;

	void set_depth_stencil(const Handle<Depth_stencil_view>& depth_stencil) const;

	static uint32_t get_attachment_type(Data_format::Value format);

	friend Rendering_device;
};

}


