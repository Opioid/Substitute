#include "Spot_shadow_renderer.hpp"
#include "Rendering/Rendering_tool.hpp"
#include "Rendering/Rendering_context.hpp"
#include "Rendering/Scene_rendering/Surface_collector.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Static_prop.hpp"
#include "Math/Vector.inl"
#include "Math/Matrix.inl"
#include "Math/Intersection/Polyhedron.hpp"

namespace rendering
{

Spot_shadow_renderer::Spot_shadow_renderer(Rendering_tool& rendering_tool, Surface_collector& surface_collector, const uint2& dimensions) :
	Shadow_renderer(rendering_tool, surface_collector, dimensions)
{}

void Spot_shadow_renderer::render(const scene::Light& light, const Frustum& light_frustum, const scene::Scene& scene, const Rendering_context& context)
{
	const auto& camera = context.camera();

	Polyhedron shadow_volume = Polyhedron::create_point_shadow_caster_volume(camera.frustum(), light.world_position());

	AABB shadow_caster_aabb = calculate_shadow_caster_AABB(scene, context.options().is_set(Rendering_context::Options::Render_actors), shadow_volume, light_frustum);

	calculate_optimized_light_view_projection(view_data_.shadow_view_projection, view_data_.linear_depth_projection, light, shadow_caster_aabb, camera.frustum());

	Frustum shadow_frustum(view_data_.shadow_view_projection);

	surface_collector_.collect_unified(scene, light.world_position(), shadow_frustum,
									   context.options().is_set(Rendering_context::Options::Render_actors),
									   context.options().is_set(Rendering_context::Options::Render_static_geometry));

	render_shadow_map(view_data_);

	rendering_tool_.device().set_viewports(1, &context.viewport());
}

const float4x4& Spot_shadow_renderer::view_projection() const
{
	return view_data_.shadow_view_projection;
}

const float2& Spot_shadow_renderer::linear_depth_projection() const
{
	return view_data_.linear_depth_projection;
}

void Spot_shadow_renderer::calculate_optimized_light_view_projection(float4x4& view_projection, float2& linear_depth_projection, const scene::Light& light, const AABB& shadow_caster_aabb, const Frustum& frustum)
{
	const float3 light_direction = light.world_direction();

	// we still need to compute a pos that is just outside the shadow_caster_aabb, but centering on the shadowReceiverAABB center with light dir
	float center = dot(shadow_caster_aabb.position - light.world_position(), light_direction);
	float radius = dot(shadow_caster_aabb.halfsize, absolute(light_direction));

	float z_near = center - radius;
	float z_far  = center + radius;

	z_near = std::max(z_near - 0.01f, 0.25f);

	view_projection = light.calculate_view_projection(z_near, z_far);

//	AABB viewAABB = frustum.calculate_AABB();

//	float3 smin = math::max(viewAABB.get_min(), shadow_caster_aabb.get_min());
//	float3 smax = math::min(viewAABB.get_max(), shadow_caster_aabb.get_max());

//	// This is the intersection between the shadow_caster_aabb and viewAABB,
//	// I believe it represents the area that potentially can recieve visible shadows
//	AABB shadowReceiverAABB(smin, smax);

//	center = dot(shadowReceiverAABB.position - light.world_position(), light_direction);
//	radius = dot(shadowReceiverAABB.halfsize, absolute(light_direction));

//	float shadowReceiverNear = std::max(center - radius, z_near);

//	//Depth clamp
//	depth_clamp.x = -0.125f;//-std::max(shadowReceiverNear / z_far, 0.f);
//	depth_clamp.y = 1.f / (1.f + depth_clamp.x);

//	std::cout << depth_clamp << std::endl;



//	depth_clamp.x = 0.f;
//	depth_clamp.y = 1.f;

	linear_depth_projection.x = z_far / (z_far - z_near);
	linear_depth_projection.y = ((-z_far * z_near) / (z_far - z_near)) / z_far;

}

AABB Spot_shadow_renderer::calculate_shadow_caster_AABB(const scene::Scene& scene, bool include_actors, const Polyhedron &shadow_volume, const Frustum& light_frustum)
{
	AABB shadow_caster_aabb(float3(1000000.f, 1000000.f, 1000000.f), float3(-1000000.f, -1000000.f, -1000000.f));

	if (include_actors)
	{
		auto& actors = scene.actors();

		for (auto a : actors)
		{
			auto& aabb = a->aabb();
			if (shadow_volume.intersect(aabb) && light_frustum.intersect(aabb))
			{
				shadow_caster_aabb = shadow_caster_aabb.merge(aabb);
		//		relevantActors.push_back(a);
			}
		}
	}

	const scene::AABB_tree& tree = scene.aabb_tree();

	{
		auto& props = tree.get_static_props();

		for (auto p : props)
		{
			auto& aabb = p->aabb();
			if (shadow_volume.intersect(aabb) && light_frustum.intersect(aabb))
			{
				shadow_caster_aabb = shadow_caster_aabb.merge(aabb);
			}
		}
	}

	const scene::AABB_node* node = tree.get_root();

//	*firstRelevantNode = node;

	while (node)
	{
		auto& node_AABB = node->aabb();

		const Intersection_type::Value intersection_v = shadow_volume.intersect(node_AABB);
		const Intersection_type::Value intersection_f = light_frustum.intersect(node_AABB);

		if (Intersection_type::Outside == intersection_v || Intersection_type::Outside == intersection_f)
		{
			node = node->get_skip_node();
			continue;
		}
		else if (Intersection_type::Inside == intersection_v && Intersection_type::Inside == intersection_f)
		{
			shadow_caster_aabb = shadow_caster_aabb.merge(node_AABB);

			//*firstRelevantNode = node;

			node = node->get_skip_node();
			continue;
		}

		auto& props = node->get_static_props();

		for (auto p : props)
		{
			auto& aabb = p->aabb();
			if (shadow_volume.intersect(aabb) && light_frustum.intersect(aabb))
			{
				shadow_caster_aabb = shadow_caster_aabb.merge(aabb);
			}
		}

		if (!node->has_children())
		{
			if (node->get_geometry())
			{
				shadow_caster_aabb = shadow_caster_aabb.merge(node_AABB);
			}

			node = node->get_skip_node();
		}
		else
		{
			++node;
		}
	}

	return shadow_caster_aabb;
}

}
