#include "Spot_shadow_renderer_EVSM_MS.hpp"
#include "Rendering/Rendering_tool.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Input_layout.hpp"
#include "Rendering/Scene_rendering/Surface_collector.hpp"

namespace rendering
{

Spot_shadow_renderer_EVSM_MS::Spot_shadow_renderer_EVSM_MS(Rendering_tool& rendering_tool, Surface_collector& surface_collector, const uint2& dimensions) :
	Spot_shadow_renderer(rendering_tool, surface_collector, dimensions)
{
	const float2 texel_size = 1.f / float2(dimensions);
	const float2 sample_offset = 0.5f * texel_size;
	const float weight = 1.f / float(filter_kernel_size_);
	filter_kernel_[0] = float4(-sample_offset.x,  sample_offset.y, weight, 0.f);
	filter_kernel_[1] = float4( sample_offset.x,  sample_offset.y, weight, 0.f);
	filter_kernel_[2] = float4(-sample_offset.x, -sample_offset.y, weight, 0.f);
	filter_kernel_[3] = float4( sample_offset.x, -sample_offset.y, weight, 0.f);
//	filter_kernel_[4] = float4( 0.f, 0.f, weight, 0.f);
}

bool Spot_shadow_renderer_EVSM_MS::init(Resource_manager& resource_manager, Constant_buffer_cache& constant_buffer_cache)
{
	Texture_description texture_description;

	texture_description.type = Texture_description::Type::Texture_2D;
	texture_description.format = Data_format::R32G32B32A32_Float;
	texture_description.dimensions.xy = shadow_map_dimensions_;
	texture_description.num_samples = 0;
	texture_description.shader_resource = true;

	auto& render_target_cache = rendering_tool_.get_render_target_cache();

	shadow_map_ = render_target_cache.get_render_target_shader_resource_view(texture_description);
	if (!shadow_map_)
	{
		return false;
	}

	if (!Shadow_renderer_EVSM_MS::init(shadow_map_dimensions_, rendering_tool_, resource_manager, constant_buffer_cache))
	{
		return false;
	}

	return true;
}

Handle<Shader_resource_view>& Spot_shadow_renderer_EVSM_MS::get_shadow_map()
{
	return shadow_map_->get_shader_resource_view();
}

uint32_t Spot_shadow_renderer_EVSM_MS::get_filter_kernel_size() const
{
	return filter_kernel_size_;
}

const float4* Spot_shadow_renderer_EVSM_MS::get_filter_kernel() const
{
	return filter_kernel_;
}

void Spot_shadow_renderer_EVSM_MS::render_shadow_map(const View_data& view_data)
{
	generate_shadow(view_data.shadow_view_projection, surface_collector_.get_surfaces(), viewport_, rendering_tool_.get_device());

	resolve_shadow_not_linear(resolve_shadow_.framebuffer, depth_->get_shader_resource_view(), view_data.linear_depth_projection, viewport_, rendering_tool_);
}

bool Spot_shadow_renderer_EVSM_MS::on_resize_targets(const uint2& /*dimensions*/, const Handle<Depth_stencil_shader_resource_view>& /*depth_stencil*/)
{
	return true;
}

}
