#pragma once

#include "Deferred_shadow_renderer.hpp"
#include "Rendering/Scene_rendering/Deferred_volume/Cascade_volume.hpp"
#include "Math/Intersection/AABB.hpp"

class Frustum;
class Polyhedron;

namespace rendering
{

class Directional_shadow_renderer : public Deferred_shadow_renderer
{

public:

	Directional_shadow_renderer(Rendering_tool& rendering_tool, Surface_collector& surface_collector, const uint2& dimensions);

	void render(const scene::Light& light, const scene::Scene& scene, const Rendering_context& context);

protected:

	struct Cascade_data;

	virtual void render_cascade(const Rendering_context& context, const Cascade_data& cascade_data) = 0;

	void calculate_optimized_light_view_projection(float4x4& view_proj, float2& depth_clamp, float4x4& volume_world, float3& virtual_eye,
												   const scene::Light& light, const AABB& shadow_caster_aabb, const Frustum& frustum, bool fixed, uint32_t cascade_index) const;

	static AABB calculate_shadow_caster_AABB(const scene::Scene& scene, bool include_actors, const Polyhedron &shadow_volume);

protected:

	struct Cascade_data
	{
		float2   near_far;

		float4x4 cascade_world;
		float4x4 shadow_view_projection;
		float2   depth_clamp;

		float3   eye;

		bool     last_cascade;
	};

	const uint32_t num_cascades_ = 4;
	Cascade_data   cascade_data_[4];

	const uint32_t filter_kernel_size_ = 4;
	float4         filter_kernel_[4];

	Cascade_volume volume_;
};

}
