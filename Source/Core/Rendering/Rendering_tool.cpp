#include "Rendering_tool.hpp"
#include "Rendering/Framebuffer.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Effect/Constant_buffer_adapter.hpp"
#include "Math/Vector.inl"

namespace rendering
{

Rendering_tool::Rendering_tool() : render_target_cache_(device_), sampler_state_cache_(device_), vertex_layout_cache_(device_), render_state_cache_(device_)
{}

bool Rendering_tool::init(const uint2& dimensions, const uint2& virtual_dimensions, bool /*windowed*/, uint32_t sync_interval, const platform::Client& client)
{
	dimensions_ = dimensions;
	virtual_dimensions_ = virtual_dimensions;

	if (!device_.init(client))
	{
		return false;
	}

	if (!swapchain_.init(client))
	{
		return false;
	}

	swapchain_.set_sync_interval(sync_interval);

	viewport_ = Viewport(float2(0.f, 0.f), float2(dimensions));

	virtual_viewport_ = Viewport(float2(0.f, 0.f), float2(virtual_dimensions));

	if (!init_render_buffer())
	{
		return false;
	}

	default_framebuffer_ = Handle<Framebuffer>(new Framebuffer);

	Texture_description texture_description;
	texture_description.type = Texture_description::Type::Texture_2D;
	texture_description.format = Data_format::D24_UNorm_S8_UInt;
	texture_description.dimensions.xy = virtual_dimensions;

	main_depth_stencil_ = device_.create_depth_stencil_shader_resource_view(texture_description);

	if (!main_depth_stencil_)
	{
		return false;
	}

	Constant_buffer_adapter::set_buffer_alignment(device_.get_constant_buffer_offset_alignment());

	return true;
}

void Rendering_tool::release()
{
	render_target_cache_.release();
	sampler_state_cache_.release();
	vertex_layout_cache_.release();
	render_state_cache_.release();
}

const uint2& Rendering_tool::dimensions() const
{
	return dimensions_;
}

const uint2& Rendering_tool::virtual_dimensions() const
{
	return virtual_dimensions_;
}

const Rendering_device& Rendering_tool::device() const
{
	return device_;
}

Rendering_device& Rendering_tool::device()
{
	return device_;
}

const Viewport& Rendering_tool::viewport() const
{
	return viewport_;
}

const Viewport& Rendering_tool::virtual_viewport() const
{
	return virtual_viewport_;
}

void Rendering_tool::present() const
{
	swapchain_.present();
}

Render_target_cache& Rendering_tool::render_target_cache()
{
	return render_target_cache_;
}

Sampler_state_cache& Rendering_tool::sampler_state_cache()
{
	return sampler_state_cache_;
}

Vertex_layout_cache& Rendering_tool::vertex_layout_cache()
{
	return vertex_layout_cache_;
}

Render_state_cache& Rendering_tool::render_state_cache()
{
	return render_state_cache_;
}

const Handle<Framebuffer>& Rendering_tool::default_framebuffer() const
{
	return default_framebuffer_;
}

const Handle<Depth_stencil_shader_resource_view> Rendering_tool::main_depth_stencil() const
{
	return main_depth_stencil_;
}

void Rendering_tool::render_fullscreen_effect()
{
	Primitive_topology topology = device_.get_primitive_topology();
	device_.set_primitive_topology(Primitive_topology::Triangle_list);
	device_.set_vertex_buffer(vertex_buffer_, sizeof(float) * 4);
	device_.draw(3);
	device_.set_primitive_topology(topology);
}

bool Rendering_tool::init_render_buffer()
{
	float vertices[] =
	{
		-1.f,  1.f, 	0.f, 2.f,	// pos, tex
		 3.f,  1.f, 	2.f, 2.f,
		-1.f, -3.f, 	0.f, 0.f
	};

	vertex_buffer_ = device_.create_vertex_buffer(sizeof(float) * 4 * 3, vertices);

	return nullptr != vertex_buffer_;
}

}
