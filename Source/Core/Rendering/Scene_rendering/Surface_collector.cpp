#include "Surface_collector.hpp"
#include "Math/Intersection/Frustum.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Actor.hpp"
#include "Scene/Static_prop.hpp"
#include "Scene/Model.hpp"
#include "Scene/AABB_tree/AABB_node.hpp"
#include <algorithm>

namespace rendering
{

Render_surface::Render_surface(const scene::Surface* surface, uint32_t model_id, uint32_t /*group_id*/, float distance) : surface(surface)
{
	static const uint32_t width_16 = 0x0000ffff;
	static const uint32_t width_20 = 0x000fffff;

	const scene::Material* material = surface->material;

	uint32_t material_part = width_20 & (material->get_property_mask() << 16 | (width_16 & material->get_manageable_id()));

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
		collect(scene.get_aabb_tree(), eye_position, frustum);
	}

	if (collect_actors)
	{
		auto& actors = scene.get_actors();

		for (auto a : actors)
		{
			if (frustum.intersect(a->get_aabb()))
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
		collect_unified(scene.get_aabb_tree(), eye_position, frustum);
	}

	if (collect_actors)
	{
		auto& actors = scene.get_actors();

		for (auto a : actors)
		{
			if (frustum.intersect(a->get_aabb()))
			{
				add_unified(a, eye_position);
			}
		}
	}

	std::sort(surfaces_.begin(), surfaces_.end());
}

const std::vector<Render_surface>& Surface_collector::get_surfaces() const
{
	return surfaces_;
}

void Surface_collector::collect(const scene::AABB_tree& tree, const float3& eye_position, const Frustum& frustum)
{
	{
		auto& props = tree.get_static_props();

		for (auto p : props)
		{
			if (frustum.intersect(p->get_aabb()))
			{
				add(p, eye_position);
			}
		}
	}

	const scene::AABB_node* node = tree.get_root();

	while (node)
	{
		if (!frustum.intersect(node->get_aabb()))
		{
			node = node->get_skip_node();
			continue;
		}

		auto& props = node->get_static_props();

		for (auto p : props)
		{
			if (frustum.intersect(p->get_aabb()))
			{
				add(p, eye_position);
			}
		}

		if (!node->has_children())
		{
			add(node, eye_position);

			node = node->get_skip_node();
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
		auto& props = tree.get_static_props();

		for (auto p : props)
		{
			if (frustum.intersect(p->get_aabb()))
			{
				add_unified(p, eye_position);
			}
		}
	}

	const scene::AABB_node* node = tree.get_root();

	while (node)
	{
		if (!frustum.intersect(node->get_aabb()))
		{
			node = node->get_skip_node();
			continue;
		}

		auto& props = node->get_static_props();

		for (auto p : props)
		{
			if (frustum.intersect(p->get_aabb()))
			{
				add_unified(p, eye_position);
			}
		}

		if (!node->has_children())
		{
			add_unified(node, eye_position);

			node = node->get_skip_node();
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

	float dist = squared_distance(node->get_aabb().position, eye_position);

	uint32_t model_id = 0;

	uint32_t num_surfaces = geometry->get_num_surfaces();
	const scene::Surface* surfaces = geometry->get_surfaces();

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

	float dist = squared_distance(node->get_aabb().position, eye_position);

	uint32_t model_id = 0;

	if (geometry->has_mixed_render_states())
	{
		uint32_t num_surfaces = geometry->get_num_surfaces();
		const scene::Surface* surfaces = geometry->get_surfaces();

		for (uint32_t i = 0; i < num_surfaces; ++i)
		{
			surfaces_.push_back(Render_surface(&surfaces[i], model_id, i, dist));
		}
	}
	else
	{
		surfaces_.push_back(Render_surface(&geometry->get_unified_surface(), model_id, 0, dist));
	}
}

void Surface_collector::add(const scene::Prop* prop, const float3& eye_position)
{
	float dist = squared_distance(prop->get_aabb().position, eye_position);

	uint32_t model_id = prop->get_model()->get_manageable_id();

	uint32_t num_surfaces = prop->get_num_surfaces();
	const scene::Surface* surfaces = prop->get_surfaces();

	for (uint32_t i = 0; i < num_surfaces; ++i)
	{
		surfaces_.push_back(Render_surface(&surfaces[i], model_id, i, dist));
	}
}

void Surface_collector::add_unified(const scene::Prop* prop, const float3& eye_position)
{
	float dist = squared_distance(prop->get_aabb().position, eye_position);

	uint32_t model_id = prop->get_model()->get_manageable_id();

	if (prop->has_mixed_render_states())
	{
		uint32_t num_surfaces = prop->get_num_surfaces();
		const scene::Surface* surfaces = prop->get_surfaces();

		for (uint32_t i = 0; i < num_surfaces; ++i)
		{
			surfaces_.push_back(Render_surface(&surfaces[i], model_id, i, dist));
		}
	}
	else
	{
		surfaces_.push_back(Render_surface(&prop->get_unified_surface(), model_id, 0, dist));
	}
}

}
