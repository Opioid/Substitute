#pragma once

#include "Main_scene_renderer.hpp"
#include "Math/Matrix.hpp"
#include "Rendering/Effect/Constant_buffer_updater.hpp"

namespace scene
{

class Material;

}

namespace rendering
{

class Framebuffer;
class Effect;
class Effect_technique;
class Input_layout;
class Rasterizer_state;
class Depth_stencil_state;
class Blend_state;

class Test_scene_renderer0 : public Main_scene_renderer
{

public:

	Test_scene_renderer0(Rendering_tool& rendering_tool, Surface_collector& surface_collector, Surrounding_renderer& surrounding_renderer);

    virtual bool init(Resource_manager& resource_manager, Constant_buffer_cache& constant_buffer_cache);

	virtual void render(const scene::Scene& scene, const Rendering_context& context);

private:

    virtual bool on_resize_targets(const uint2& dimensions, const Handle<Depth_stencil_shader_resource_view>& depth_stencil);

	bool prepare_material(const scene::Material* material);

	bool create_render_states();

	Handle<Effect> effect_;

	Handle<Input_layout> input_layout_;

	const scene::Material* previous_material_;
	uint32_t previous_technique_;

	Handle<Rasterizer_state> rasterizer_state_cull_back_;

	Handle<Depth_stencil_state> base_ds_state_;

	Handle<Blend_state> base_blend_state_;

	struct Change_per_object
	{
		float4x4 world_view_projection;
		float4x4 world_view;
	};

	Constant_buffer_updater<Change_per_object> change_per_object_;
};

}


