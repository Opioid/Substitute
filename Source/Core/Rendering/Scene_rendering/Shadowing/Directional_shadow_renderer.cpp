#include "Directional_shadow_renderer.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Rendering_context.hpp"
#include "Rendering/Scene_rendering/Surface_collector.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Actor.hpp"
#include "Scene/Static_prop.hpp"
#include "Math/Vector.inl"
#include "Math/Matrix.inl"
#include "Math/Intersection/Polyhedron.hpp"

#include <iostream>

namespace rendering
{

Directional_shadow_renderer::Directional_shadow_renderer(Rendering_tool &rendering_tool, Surface_collector& surface_collector, const uint2& dimensions) :
	Deferred_shadow_renderer(rendering_tool, surface_collector, dimensions)
{
	for (uint32_t i = 0; i < num_cascades_; ++i)
	{
		cascade_data_[i].last_cascade = i == num_cascades_ - 1;
	}

	const float2 texel_size = 1.f / float2(dimensions);
	const float2 sample_offset = 0.5f * texel_size;
	const float weight = 1.f / float(filter_kernel_size_);
	filter_kernel_[0] = float4(-sample_offset.x,  sample_offset.y, weight, 0.f);
	filter_kernel_[1] = float4( sample_offset.x,  sample_offset.y, weight, 0.f);
	filter_kernel_[2] = float4(-sample_offset.x, -sample_offset.y, weight, 0.f);
	filter_kernel_[3] = float4( sample_offset.x, -sample_offset.y, weight, 0.f);
//	filter_kernel_[4] = float4( 0.f, 0.f, weight, 0.f);
}

void Directional_shadow_renderer::render(const scene::Light& light, const scene::Scene& scene, const Rendering_context& context)
{
	const auto& camera = context.camera();

	cascade_data_[0].near_far = float2(camera.z_near(), 4.f);
	cascade_data_[1].near_far = float2(4.f, 10.f);
	cascade_data_[2].near_far = float2(10.f, 24.f);
	cascade_data_[3].near_far = float2(camera.z_near(), camera.z_far());

	Polyhedron shadow_volume = Polyhedron::create_directional_shadow_caster_volume(camera.frustum(), light.world_direction());

	AABB shadow_caster_aabb = calculate_shadow_caster_AABB(scene, context.options().is_set(Rendering_context::Options::Render_actors), shadow_volume);

	for (uint32_t i = 0; i < num_cascades_; ++i)
	{
		Cascade_data& cascade_data = cascade_data_[i];

		scene::Camera cascade_camera = camera;
		cascade_camera.set_projection(camera.fov(), camera.ratio(), cascade_data.near_far.x, cascade_data.near_far.y);
		cascade_camera.update_frustum();

		calculate_optimized_light_view_projection(cascade_data.shadow_view_projection, cascade_data.depth_clamp, cascade_data.cascade_world, cascade_data.eye,
												  light, shadow_caster_aabb, cascade_camera.frustum(), !cascade_data.last_cascade, i);

		Frustum shadow_frustum(cascade_data.shadow_view_projection);

		surface_collector_.collect_unified(scene, cascade_data.eye, shadow_frustum,
										   context.options().is_set(Rendering_context::Options::Render_actors),
										   context.options().is_set(Rendering_context::Options::Render_static_geometry));

		render_cascade(context, cascade_data);
	}
}

void Directional_shadow_renderer::calculate_optimized_light_view_projection(float4x4& view_proj, float2& depth_clamp, float4x4& cascade_world, float3& virtual_eye,
																			const scene::Light& light, const AABB& shadow_caster_aabb, const Frustum& frustum, bool fixed, uint32_t /*cascade_index*/) const
{
	AABB viewAABB = frustum.calculate_AABB();

	float3 smin = math::max(viewAABB.get_min(), shadow_caster_aabb.get_min());
	float3 smax = math::min(viewAABB.get_max(), shadow_caster_aabb.get_max());

	//This is the intersection between the shadow_caster_aabb and viewAABB,
	//I believe it represents the area that potentially can recieve visible shadows
	AABB tightShadowReceiverAABB(smin, smax);
	AABB shadowReceiverAABB = tightShadowReceiverAABB;

	if (fixed)
	{
		// Using a fixed size solves shadow swimming for camera rotations!(?)
		float size = 0.5f * distance(frustum.points_[0], frustum.points_[7]);
		size = 0.01f * floor(100.f * size + 0.5f);

		shadowReceiverAABB.halfsize = float3(size, size, size);
	}

	const float3 light_direction = light.world_direction();

	// We still need to compute a pos that is just outside the shadow_caster_aabb, but centering on the shadowReceiverAABB center with light dir
	float center = dot(shadow_caster_aabb.position - shadowReceiverAABB.position, light_direction);
	float radius = dot(shadow_caster_aabb.halfsize, absolute(light_direction));

	float znear = center - radius;

	virtual_eye = shadowReceiverAABB.position + znear * light_direction;
	float4x4 view;
	set_look_at(view, virtual_eye, virtual_eye + light_direction, light.world_up());

	center = dot(tightShadowReceiverAABB.position - virtual_eye, light_direction);
	radius = dot(tightShadowReceiverAABB.halfsize, absolute(light_direction));

	float shadowRecieverNear = center - radius;

	center = dot(shadowReceiverAABB.position - virtual_eye, light_direction);
	radius = dot(shadowReceiverAABB.halfsize, absolute(light.world_direction()));

	float z_far = center + radius;

	AABB transformedShadowReceiverAABB;
	shadowReceiverAABB.get_transformed(transformedShadowReceiverAABB, view);

	smin = transformedShadowReceiverAABB.get_min();
	smax = transformedShadowReceiverAABB.get_max();

	float width = smax.x - smin.x;
	float height = smax.y - smin.y;

	float4x4 proj;
	set_ortho(proj, width, height, 0.f, z_far);

	view_proj = view * proj;

	// Depth clamp

	depth_clamp.x = std::max(shadowRecieverNear / z_far, 0.f);
	depth_clamp.y = 1.f / (1.f - depth_clamp.x);

	// Reduce shadow swimming caused by camera translation
	const float shadow_resolution = float(shadow_map_dimensions_.x);
	const float half_shadow_resolution = shadow_resolution * 0.5f;

	float2 texCoords(view_proj.m30 * half_shadow_resolution, view_proj.m31 * half_shadow_resolution);
	float2 d = (round(texCoords) - texCoords) / half_shadow_resolution;

	view_proj.m30 += d.x;
	view_proj.m31 += d.y;

	if (fixed)
	{
		// We know that fixed means "not the last cascade", so we need the world transformation matrix for the cascade volume

		float3 vscale(smax.x - smin.x, smax.y - smin.y, z_far - shadowRecieverNear);

		const float texel_size = 1.f / shadow_resolution;
		const float3 shrink = vscale * (5.f * float3(texel_size, texel_size, texel_size));

		vscale -= float3(shrink.x, shrink.y, shrink.z);

		cascade_world = float4x4::identity;

		set_basis(cascade_world, light.world_rotation());
		scale(cascade_world, vscale);
		set_origin(cascade_world, virtual_eye + (shadowRecieverNear + 0.5f * shrink.z) * light.world_direction());
	}
}

AABB Directional_shadow_renderer::calculate_shadow_caster_AABB(const scene::Scene& scene, bool include_actors, const Polyhedron &shadow_volume)
{
	AABB shadow_caster_aabb(float3(1000000.f, 1000000.f, 1000000.f), float3(-1000000.f, -1000000.f, -1000000.f));

	if (include_actors)
	{
		auto& actors = scene.actors();

		for (auto a : actors)
		{
			auto& aabb = a->aabb();
			if (Intersection_type::Outside != shadow_volume.intersect(aabb))
			{
				shadow_caster_aabb = shadow_caster_aabb.merge(aabb);
		//		relevantActors.push_back(a);
			}
		}
	}

	const scene::AABB_tree& tree = scene.aabb_tree();

	{
		auto& props = tree.static_props();

		for (auto p : props)
		{
			auto& aabb = p->aabb();
			if (Intersection_type::Outside != shadow_volume.intersect(aabb))
			{
				shadow_caster_aabb = shadow_caster_aabb.merge(aabb);
			}
		}
	}

	const scene::AABB_node* node = tree.root();

//	*firstRelevantNode = node;

	while (node)
	{
		auto& node_AABB = node->aabb();

		const Intersection_type intersection = shadow_volume.intersect(node_AABB);

		if (Intersection_type::Outside == intersection)
		{
			node = node->skip_node();
			continue;
		}
		else if (Intersection_type::Inside == intersection)
		{
			shadow_caster_aabb = shadow_caster_aabb.merge(node_AABB);

			//*firstRelevantNode = node;

			node = node->skip_node();
			continue;
		}

		auto& props = node->static_props();

		for (auto p : props)
		{
			auto& aabb = p->aabb();
			if (Intersection_type::Outside != shadow_volume.intersect(aabb))
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

			node = node->skip_node();
		}
		else
		{
			++node;
		}
	}

	return shadow_caster_aabb;
}

}
