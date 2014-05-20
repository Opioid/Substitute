#pragma once

#include "Shadow_renderer.hpp"
#include "Math/Intersection/AABB.hpp"

class Frustum;
class Polyhedron;

namespace rendering
{

class Spot_shadow_renderer : public Shadow_renderer
{

public:

	Spot_shadow_renderer(Rendering_tool& rendering_tool, Surface_collector& surface_collector, const uint2& dimensions);

	void render(const scene::Light& light, const Frustum& light_frustum, const scene::Scene& scene, const Rendering_context& context);

	const float4x4& get_view_projection() const;

	const float2& get_linear_depth_projection() const;

	virtual uint32_t get_filter_kernel_size() const = 0;

	virtual const float4* get_filter_kernel() const = 0;

protected:

	struct View_data;

	virtual void render_shadow_map(const View_data& view_data) = 0;

	static void calculate_optimized_light_view_projection(float4x4& view_projection, float2& linear_depth_projection, const scene::Light& light, const AABB& shadow_caster_aabb, const Frustum& frustum);

	static AABB calculate_shadow_caster_AABB(const scene::Scene& scene, bool include_actors, const Polyhedron& shadow_volume, const Frustum& light_frustum);

	struct View_data
	{
		float4x4 shadow_view_projection;
		float2   linear_depth_projection;
	};

	View_data view_data_;
};

}
