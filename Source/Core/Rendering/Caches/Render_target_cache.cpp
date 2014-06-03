#include "Render_target_cache.hpp"
#include "Rendering/Rendering_device.hpp"
#include "Rendering/Resource_view.hpp"

namespace rendering
{

Render_target_cache::Render_target_cache(const Rendering_device& device) : Cache(device)
{}

void Render_target_cache::release()
{
	Render_target_views_.clear();
	depth_stencil_views_.clear();
}

Handle<Depth_stencil_view> Render_target_cache::depth_stencil_view(const Texture_description& description,
																   std::initializer_list<Handle<Depth_stencil_view>> must_not_match)
{
	for (const auto& t : depth_stencil_views_)
	{
		if (t.description == description)
		{
			bool unique = true;

			for (const auto& other : must_not_match)
			{
				if (t.resource == other)
				{
					unique = false;
					continue;
				}
			}

			if (!unique)
			{
				continue;
			}

			return t.resource;
		}
	}

	Handle<Depth_stencil_view> depth_stencil = device_.create_depth_stencil_view(description);

	Resource_slot<Depth_stencil_view> slot{ description, depth_stencil };

	depth_stencil_views_.push_back(slot);

	return depth_stencil;
}

Handle<Depth_stencil_shader_resource_view> Render_target_cache::depth_stencil_shader_resource_view(const Texture_description& description,
																								   std::initializer_list<Handle<Depth_stencil_shader_resource_view>> must_not_match)
{
	for (const auto& t : depth_stencil_shader_resource_views_)
	{
		if (t.description == description)
		{
			bool unique = true;

			for (const auto& other : must_not_match)
			{
				if (t.resource == other)
				{
					unique = false;
					continue;
				}
			}

			if (!unique)
			{
				continue;
			}

			return t.resource;
		}
	}

	Handle<Depth_stencil_shader_resource_view> depth_stencil_shader_resource = device_.create_depth_stencil_shader_resource_view(description);

	Resource_slot<Depth_stencil_shader_resource_view> slot{ description, depth_stencil_shader_resource };

	depth_stencil_shader_resource_views_.push_back(slot);

	return depth_stencil_shader_resource;
}

Handle<Render_target_shader_resource_view> Render_target_cache::render_target_shader_resource_view(const Texture_description& description,
																								   std::initializer_list<Handle<Render_target_shader_resource_view>> must_not_match)
{
	for (const auto& t : Render_target_views_)
	{
		if (t.description == description)
		{
			bool unique = true;

			for (const auto& other : must_not_match)
			{
				if (t.resource == other)
				{
					unique = false;
					continue;
				}
			}

			if (!unique)
			{
				continue;
			}

			return t.resource;
		}
	}

	Handle<Render_target_shader_resource_view> render_target = device_.create_render_target_shader_resource_view(description);

	Resource_slot<Render_target_shader_resource_view> slot{ description, render_target };

	Render_target_views_.push_back(slot);

	return render_target;
}

}
