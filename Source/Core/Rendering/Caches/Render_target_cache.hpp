#pragma once

#include "Cache.hpp"
#include "Rendering/Texture_description.hpp"
#include "Resources/Handle.hpp"
#include <list>
#include <initializer_list>

namespace rendering
{

class Depth_stencil_view;
class Depth_stencil_shader_resource_view;
class Render_target_shader_resource_view;

class Render_target_cache : public Cache
{

public:

	Render_target_cache(const Rendering_device& device);

	virtual void release();

	Handle<Depth_stencil_view> depth_stencil_view(const Texture_description& description,
													  std::initializer_list<Handle<Depth_stencil_view>> must_not_match = {});

	Handle<Depth_stencil_shader_resource_view> get_depth_stencil_shader_resource_view(const Texture_description& description,
																					  std::initializer_list<Handle<Depth_stencil_shader_resource_view>> must_not_match = {});

	Handle<Render_target_shader_resource_view> get_render_target_shader_resource_view(const Texture_description& description,
																					  std::initializer_list<Handle<Render_target_shader_resource_view>> must_not_match = {});

private:

	template<typename T>
	struct Resource_slot
	{
		Texture_description description;
		Handle<T> resource;
	};

	std::list<Resource_slot<Depth_stencil_view>> depth_stencil_views_;

	std::list<Resource_slot<Depth_stencil_shader_resource_view>> depth_stencil_shader_resource_views_;

	std::list<Resource_slot<Render_target_shader_resource_view>> render_tarviews_;
};

}


