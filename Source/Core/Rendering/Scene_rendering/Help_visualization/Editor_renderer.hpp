#pragma once

#include "Rendering/Scene_rendering/Scene_renderer.hpp"
#include "Rendering/Effect/Constant_buffer_updater.hpp"
#include "Math/Matrix.hpp"

namespace scene
{

class Scene;
class Editor;

}

namespace rendering
{

class Rendering_context;
class Effect;
class Effect_technique;
class Input_layout;

class Editor_renderer : public Scene_renderer
{

public:

	Editor_renderer(Rendering_tool& rendering_tool);
	virtual ~Editor_renderer();

	virtual bool init(Resource_manager& resource_manager, Constant_buffer_cache& constant_buffer_cache);

	void render(const scene::Scene& scene, const scene::Editor& editor, const Rendering_context& context);

private:

	virtual bool on_resize_targets(const uint2& size, const Handle<Depth_stencil_shader_resource_view>& depth_stencil);

	bool init_buffers();

	bool create_render_states();

	Handle<Effect> effect_;

	Effect_technique* per_shape_color_technique_;
	Effect_technique* per_vertex_color_technique_;
	Effect_technique* shape_technique_;

	Handle<Input_layout> per_shape_color_input_layout_;
	Handle<Input_layout> per_vertex_color_input_layout_;
	Handle<Input_layout> shape_input_layout_;

	struct Change_per_camera
	{
		float4x4 view_projection;
		float4x4 view;
	};

	Constant_buffer_updater<Change_per_camera> change_per_camera_;

	struct Change_per_object
	{
		float4x4 world;
		float3   color;
	};

	Constant_buffer_updater<Change_per_object> change_per_object_;

	Handle<Vertex_buffer> vertex_buffer_;

	Handle<Rasterizer_state>    rasterizer_state_;
	Handle<Depth_stencil_state> ds_state_;
	Handle<Blend_state>         blend_state_;

	class Shape
	{

	public:

		void render(Rendering_tool& rendering_tool) const;

	protected:

		Handle<Vertex_buffer> vertex_buffer_;
		Handle<Index_buffer>  index_buffer_;

		uint32_t num_indices_;
	};

	class Cone : public Shape
	{

	public:

		bool init(Rendering_tool& rendering_tool);
	};

	class Cylinder : public Shape
	{

	public:

		bool init(Rendering_tool& rendering_tool);
	};

	class Sphere : public Shape
	{

	public:

		bool init(Rendering_tool& rendering_tool);
	};

	class Line : public Shape
	{

	public:

		bool init(Rendering_tool& rendering_tool);
	};

	Cone     cone_;
	Cylinder cylinder_;
	Sphere	 sphere_;
	Line     line_;
};

}
