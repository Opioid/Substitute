#pragma once

#include "Rendering/Rendering_context.hpp"
#include "Resources/Handle.hpp"
#include "Math/Vector.hpp"
#include "Scene/Camera.hpp"

class Resource_manager;

namespace scene
{

class Scene;

}

namespace rendering
{

class Rendering_tool;
class Main_scene_renderer;
class Render_target_shader_resource_view;
class Cube_render_target_shader_resource_view;
class Depth_stencil_shader_resource_view;

namespace baking
{

class Environment_map_renderer
{

public:

	Environment_map_renderer(Rendering_tool& rendering_tool, Main_scene_renderer& scene_renderer);

	bool allocate_targets(const uint2& dimensions);
	void free_targets();

	void configure_for_single_cube_map(const uint2& face_dimensions);

	void configure_for_flattened_cube_map_batch(const uint2& face_dimensions);

	const uint2& target_dimensions() const;

	Handle<Framebuffer>& framebuffer();
	Handle<Cube_render_target_shader_resource_view>& color_target();

	void render(const scene::Scene& scene, const float3& position, const Rendering_context::Rendering_options& options);

	void start_batch(const Rendering_context::Rendering_options& options);

	void render(const scene::Scene& scene, const float3& position, uint32_t batch_index);

	Main_scene_renderer& scene_renderer() const;

private:

	Rendering_tool& rendering_tool_;

	Main_scene_renderer& scene_renderer_;

	Rendering_context context_;

	Handle<Framebuffer> framebuffer_;

	scene::Camera camera_;

	uint2 face_dimensions_;
	uint2 target_dimensions_;

	Handle<Cube_render_target_shader_resource_view> color_target_;
	Handle<Depth_stencil_shader_resource_view> depth_stencil_;

	static const float3 s_directions[6];
	static const float3 s_ups[6];
};

}

}


