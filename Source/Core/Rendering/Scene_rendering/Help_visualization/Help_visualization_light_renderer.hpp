#pragma once

#include "Rendering/Scene_rendering/Scene_renderer.hpp"
#include "Rendering/Input_layout.hpp"
#include "Rendering/Effect/Constant_buffer_updater.hpp"
#include "Math/Matrix.hpp"

namespace scene
{

class Scene;
class Camera;
class Model;
class Irradiance_volume;

}

namespace rendering
{

class Rendering_context;
class Effect;
class Effect_technique;
class Input_layout;

class Help_visualization_light_renderer : public Scene_renderer
{

public:

	Help_visualization_light_renderer(Rendering_tool& rendering_tool);
	virtual ~Help_visualization_light_renderer();

	virtual bool init(Resource_manager& resource_manager, Constant_buffer_cache& constant_buffer_cache);

	void render(const scene::Scene& scene, const Rendering_context& context);

private:

	virtual bool on_resize_targets(const uint2& size, const Handle<Depth_stencil_shader_resource_view>& depth_stencil);

	void render(const scene::Irradiance_volume& volume, const scene::Camera& camera);

	void render_diffuse_probe(const float3& position, const float4x4& view);

	bool create_render_states();

	Handle<scene::Model> sphere_model_;

	Handle<Effect> effect_;

	Effect_technique* light_probe_technique_;

	Handle<Input_layout> input_layout_;

	struct Change_per_camera
	{
		float4x4 view_projection;
	};

	Constant_buffer_updater<Change_per_camera> change_per_camera_;

	struct Change_per_light
	{
		float4x4 light_data;
	};

	Constant_buffer_updater<Change_per_light> change_per_light_;

	struct Change_per_object
	{
		float4x4 world;
		float3   position_vs;
	};

	Constant_buffer_updater<Change_per_object> change_per_object_;

	Handle<Rasterizer_state>    rasterizer_state_;
	Handle<Depth_stencil_state> ds_state_;
	Handle<Blend_state>         blend_state_;
};

}

