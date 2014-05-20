#pragma once

#include "Spot_shadow_renderer.hpp"
#include "Shadow_renderer_EVSM_MS.hpp"

namespace rendering
{

class Spot_shadow_renderer_EVSM_MS : public Spot_shadow_renderer, Shadow_renderer_EVSM_MS
{

public:

	Spot_shadow_renderer_EVSM_MS(Rendering_tool& rendering_tool, Surface_collector& surface_collector, const uint2& dimensions);

	virtual bool init(Resource_manager& resource_manager, Constant_buffer_cache& constant_buffer_cache);

	virtual Handle<Shader_resource_view>& get_shadow_map();

	uint32_t get_filter_kernel_size() const;

	virtual const float4* get_filter_kernel() const;

private:

	virtual void render_shadow_map(const View_data& view_data);

	virtual bool on_resize_targets(const uint2& dimensions, const Handle<Depth_stencil_shader_resource_view>& depth_stencil);

	const uint32_t filter_kernel_size_ = 4;
	float4         filter_kernel_[4];
};

}
