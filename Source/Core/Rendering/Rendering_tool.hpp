#pragma once

#include "Rendering/Swapchain.hpp"
#include "Rendering/Rendering_device.hpp"
#include "Rendering/Viewport.hpp"
#include "Rendering/Framebuffer.hpp"
#include "Caches/Render_target_cache.hpp"
#include "Caches/Sampler_state_cache.hpp"
#include "Caches/Vertex_layout_cache.hpp"
#include "Caches/Render_state_cache.hpp"
#include "Math/Vector.hpp"

namespace platform { class Client; }

namespace rendering
{

class Rendering_tool
{

public:

	Rendering_tool();

	bool init(const uint2& dimensions, const uint2& virtual_dimensions, bool windowed, uint32_t sync_interval, const platform::Client& client);
	void release();

	const uint2& dimensions() const;
	const uint2& virtual_dimensions() const;

	const Rendering_device& device() const;
	Rendering_device& device();

	const Viewport& viewport() const;
	const Viewport& virtual_viewport() const;

	void present() const;

	Render_target_cache& render_target_cache();
	Sampler_state_cache& sampler_state_cache();
	Vertex_layout_cache& vertex_layout_cache();
	Render_state_cache&  render_state_cache();

	const Handle<Framebuffer>& default_framebuffer() const;

	const Handle<Depth_stencil_shader_resource_view> main_depth_stencil() const;

	void render_fullscreen_effect();

private:

	bool init_render_buffer();

	uint2 dimensions_;
	uint2 virtual_dimensions_;

	Swapchain swapchain_;

	Rendering_device device_;

	Viewport viewport_;
	Viewport virtual_viewport_;

	Render_target_cache render_target_cache_;
	Sampler_state_cache sampler_state_cache_;
	Vertex_layout_cache vertex_layout_cache_;
	Render_state_cache  render_state_cache_;

	Handle<Framebuffer> default_framebuffer_;

	Handle<Depth_stencil_shader_resource_view> main_depth_stencil_;

	Handle<Vertex_buffer> vertex_buffer_;
};

}
