#pragma once

#include "Main_scene_renderer.hpp"
#include "Lighting/Deferred_lighting_renderer.hpp"
#include "Particle_rendering/Particle_renderer.hpp"
#include "Math/Matrix.hpp"
#include "Rendering/Effect/Constant_buffer_updater.hpp"
#include "Rendering/Constant_buffer_cache.hpp"

namespace scene
{

class Material;
class Light;
class Irradiance_volume;
class Light_probe;

}

namespace rendering
{

class Framebuffer;
class Effect;
class Effect_technique;
class Input_layout;
class Render_target_shader_resource_view;

class Deferred_scene_renderer : public Main_scene_renderer
{

public:

	Deferred_scene_renderer(Rendering_tool& rendering_tool,
							Surface_collector& surface_collector,
							Surrounding_renderer& surrounding_renderer,
							Directional_shadow_renderer& directional_shadow_renderer,
							Spot_shadow_renderer& spot_shadowr_renderer);

	virtual bool init(Resource_manager& resource_manager, Constant_buffer_cache& constant_buffer_cache);

	virtual void render(const scene::Scene& scene, const Rendering_context& context);

private:

	virtual bool on_resize_targets(const uint2& size, const Handle<Depth_stencil_shader_resource_view>& depth_stencil);

	void render_deferred_effects(const scene::Scene& scene, const Rendering_context& context);

	void prepare_material(const scene::Material* material);

	bool create_render_states();

	Deferred_lighting_renderer lighting_renderer_;

	Particle_renderer particle_renderer_;

	Handle<Render_target_shader_resource_view> normal_target_;
	Handle<Render_target_shader_resource_view> color_target_;
	Handle<Render_target_shader_resource_view> surface_target_;

	Handle<Framebuffer> framebuffer_;

	Handle<Effect> effect_;

	Handle<Input_layout> input_layout_;

	const scene::Material* previous_material_;
	uint32_t previous_technique_;
	uint32_t previous_rasterizer_state_;

	Handle<Rasterizer_state> rasterizer_state_cull_back_;
	Handle<Rasterizer_state> rasterizer_state_cull_front_;
	Handle<Rasterizer_state> rasterizer_state_cull_none_;

	enum class Cull_state
	{
		Back  = 0,
		Front = 1
	};

	Handle<Rasterizer_state> base_rasterizer_states_[2];

	Handle<Depth_stencil_state> base_ds_state_;

	Handle<Blend_state> base_blend_state_;

	Constant_buffer_cache constant_buffer_cache_;

	struct Change_per_camera
	{
		float4   rays[3];
		float4x4 view_projection;
		float4x4 view;
		float4x4 inverse_view;
	};

	Constant_buffer_updater<Change_per_camera> change_per_camera_;

	struct Change_per_object
	{
		float4x4 world;
	};

	Constant_buffer_updater<Change_per_object> change_per_object_;

	struct Change_per_material
	{
		float4 color_and_emissive_factor;
		float2 metallic_and_roughness;
		float2 height_scale;
	};

	Constant_buffer_updater<Change_per_material> change_per_material_;

	struct Change_per_frame
	{
		float3 ray0; float padding0;
		float3 ray1; float padding1;
		float3 ray2; float padding2;
		float4x4 inverse_view;
	};
};

}


