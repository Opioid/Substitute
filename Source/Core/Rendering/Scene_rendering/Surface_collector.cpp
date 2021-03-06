#include "Surface_collector.hpp"
#include "Math/Intersection/Frustum.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Actor.hpp"
#include "Scene/Static_prop.hpp"
#include "Scene/Material.hpp"
#include "Scene/Model.hpp"
#include "Scene/AABB_tree/AABB_node.hpp"
#include <algorithm>

namespace rendering
{

Render_surface::Render_surface(const scene::Surface* surface, uint32_t /*model_id*/, uint32_t /*group_id*/, float distance) : surface(surface)
{
	static const uint32_t width_16 = 0x0000FFFF;
	static const uint32_t width_20 = 0x000FFFFF;

	const scene::Material* material = surface->material;

	uint32_t material_part = width_20 & (material->property_mask() << 16 | (width_16 & material->manageable_id()));

	signature = uint64_t(material_part) << 32 | uint32_t(distance * 1000.f);
}

bool Render_surface::operator<(const Render_surface& surface) const
{
	return signature < surface.signature;
}

void Surface_collector::collect(const scene::Scene& scene, const float3& eye_position, const Frustum& frustum, bool collect_actors, bool collect_static_geometry)
{
	surfaces_.clear();

	if (collect_static_geometry)
	{
		collect(scene.aabb_tree(), eye_position, frustum);
	}

	if (collect_actors)
	{
		auto& actors = scene.actors();

		for (auto a : actors)
		{
			if (Intersection_type::Outside != frustum.intersect(a->aabb()))
			{
				add(a, eye_position);
			}
		}
	}

	std::sort(surfaces_.begin(), surfaces_.end());
}

void Surface_collector::collect_unified(const scene::Scene& scene, const float3& eye_position, const Frustum& frustum, bool collect_actors, bool collect_static_geometry)
{
	surfaces_.clear();

	if (collect_static_geometry)
	{
		collect_unified(scene.aabb_tree(), eye_position, frustum);
	}

	if (collect_actors)
	{
		auto& actors = scene.actors();

		for (auto a : actors)
		{
			if (Intersection_type::Outside != frustum.intersect(a->aabb()))
			{
				add_unified(a, eye_position);
			}
		}
	}

	std::sort(surfaces_.begin(), surfaces_.end());
}

const std::vector<Render_surface>& Surface_collector::surfaces() const
{
	return surfaces_;
}

void Surface_collector::collect(const scene::AABB_tree& tree, const float3& eye_position, const Frustum& frustum)
{
	{
		auto& props = tree.static_props();

		for (auto p : props)
		{
			if (Intersection_type::Outside != frustum.intersect(p->aabb()))
			{
				add(p, eye_position);
			}
		}
	}

	const scene::AABB_node* node = tree.root();

	while (node)
	{
		if (Intersection_type::Outside == frustum.intersect(node->aabb()))
		{
			node = node->skip_node();
			continue;
		}

		auto& props = node->static_props();

		for (auto p : props)
		{
			if (Intersection_type::Outside != frustum.intersect(p->aabb()))
			{
				add(p, eye_position);
			}
		}

		if (!node->has_children())
		{
			add(node, eye_position);

			node = node->skip_node();
		}
		else
		{
			++node;
		}
	}
}

void Surface_collector::collect_unified(const scene::AABB_tree& tree, const float3& eye_position, const Frustum& frustum)
{
	{
		auto& props = tree.static_props();

		for (auto p : props)
		{
			if (Intersection_type::Outside != frustum.intersect(p->aabb()))
			{
				add_unified(p, eye_position);
			}
		}
	}

	const scene::AABB_node* node = tree.root();

	while (node)
	{
		if (Intersection_type::Outside == frustum.intersect(node->aabb()))
		{
			node = node->skip_node();
			continue;
		}

		auto& props = node->static_props();

		for (auto p : props)
		{
			if (Intersection_type::Outside != frustum.intersect(p->aabb()))
			{
				add_unified(p, eye_position);
			}
		}

		if (!node->has_children())
		{
			add_unified(node, eye_position);

			node = node->skip_node();
		}
		else
		{
			++node;
		}
	}
}

void Surface_collector::add(const scene::AABB_node* node, const float3& eye_position)
{
	const scene::AABB_node::Geometry* geometry = node->get_geometry();

	if (!geometry)
	{
		return;
	}

	float dist = squared_distance(node->aabb().position, eye_position);

	uint32_t model_id = 0;

	uint32_t num_surfaces = geometry->num_surfaces();
	const scene::Surface* surfaces = geometry->surfaces();

	for (uint32_t i = 0; i < num_surfaces; ++i)
	{
		surfaces_.push_back(Render_surface(&surfaces[i], model_id, i, dist));
	}
}

void Surface_collector::add_unified(const scene::AABB_node* node, const float3& eye_position)
{
	const scene::AABB_node::Geometry* geometry = node->get_geometry();

	if (!geometry)
	{
		return;
	}

	float dist = squared_distance(node->aabb().position, eye_position);

	uint32_t model_id = 0;

	if (geometry->has_mixed_render_states())
	{
		uint32_t num_surfaces = geometry->num_surfaces();
		const scene::Surface* surfaces = geometry->surfaces();

		for (uint32_t i = 0; i < num_surfaces; ++i)
		{
			surfaces_.push_back(Render_surface(&surfaces[i], model_id, i, dist));
		}
	}
	else
	{
		surfaces_.push_back(Render_surface(&geometry->unified_surface(), model_id, 0, dist));
	}
}

void Surface_collector::add(const scene::Prop* prop, const float3& eye_position)
{
	float dist = squared_distance(prop->aabb().position, eye_position);

	uint32_t model_id = prop->model()->manageable_id();

	uint32_t num_surfaces = prop->num_surfaces();
	const scene::Surface* surfaces = prop->surfaces();

	for (uint32_t i = 0; i < num_surfaces; ++i)
	{
		surfaces_.push_back(Render_surface(&surfaces[i], model_id, i, dist));
	}
}

void Surface_collector::add_unified(const scene::Prop* prop, const float3& eye_position)
{
	float dist = squared_distance(prop->aabb().position, eye_position);

	uint32_t model_id = prop->model()->manageable_id();

	if (prop->has_mixed_render_states())
	{
		uint32_t num_surfaces = prop->num_surfaces();
		const scene::Surface* surfaces = prop->surfaces();

		for (uint32_t i = 0; i < num_surfaces; ++i)
		{
			surfaces_.push_back(Render_surface(&surfaces[i], model_id, i, dist));
		}
	}
	else
	{
		surfaces_.push_back(Render_surface(&prop->unified_surface(), model_id, 0, dist));
	}
}

}
