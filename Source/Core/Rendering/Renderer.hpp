#pragma once

#include "Scene_rendering/Surface_collector.hpp"
#include "Rendering_context.hpp"
#include "Post_processing/Post_processing_pipeline.hpp"

template<typename T> struct tVector2;
typedef tVector2<uint32_t> uint2;

class Application;
class Resource_manager;

namespace scene
{

class Scene;

}

namespace rendering
{

class Directional_shadow_renderer;
class Spot_shadow_renderer;
class Surrounding_renderer;
class Main_scene_renderer;
class Framebuffer;
class Render_target_shader_resource_view;
class Depth_stencil_view;
class Hdr_filter;
class Fxaa;
class Rendering_tool;
class Vertex_buffer;
class Index_buffer;
class Sampler_state;
class Rasterizer_state;
class Depth_stencil_state;
class Bounding_renderer;
class Editor_renderer;
class Help_visualization_light_renderer;

namespace baking
{

class Light_baker;

}

class Renderer
{

public:

	Renderer(Rendering_tool& rendering_tool);

	bool init(Application& app, Resource_manager& resource_manager, bool enable_fxaa);
	void release();

	baking::Light_baker* get_light_baker();

	void render(Application& app, float speed);

	bool get_render_analytical_lighting() const;
	void set_render_analytical_lighting(bool enable);

	bool get_render_image_based_lighting() const;
	void set_render_image_based_lighting(bool enable);

	bool get_render_bounding_volumes() const;
	void set_render_bounding_volumes(bool enable);

	bool get_render_help_light() const;
	void set_render_help_light(bool enable);

private:

	Rendering_tool& rendering_tool_;

	Surface_collector surface_collector_;

	Directional_shadow_renderer* directional_shadow_renderer_;
	Spot_shadow_renderer* spot_shadow_renderer_;

	Surrounding_renderer* surrounding_renderer_;

	Main_scene_renderer* scene_renderer_;

	Rendering_context hdr_context_;
	Rendering_context presentation_context_;

	Handle<Framebuffer> framebuffer_;
	Handle<Render_target_shader_resource_view> hdr_target_;
	Handle<Depth_stencil_view> depth_stencil_;

	Post_processing_pipeline post_processing_;

	Hdr_filter* hdr_filter_;
	Fxaa* fxaa_;

	baking::Light_baker* light_baker_;

	bool render_analytical_lighting_;
	bool render_image_based_lighting_;

	bool render_bounding_volumes_;
	Bounding_renderer* bounding_renderer_;

	Editor_renderer* editor_renderer_;

	bool render_help_light_;
	Help_visualization_light_renderer* help_light_renderer_;
};

}
