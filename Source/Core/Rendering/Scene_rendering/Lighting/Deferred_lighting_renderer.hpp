#pragma once

#include "Rendering/Rendering_object.hpp"
#include "Rendering/Scene_rendering/Deferred_volume/Box_volume.hpp"
#include "Rendering/Scene_rendering/Deferred_volume/Sphere_volume.hpp"
#include "Rendering/Scene_rendering/Deferred_volume/Frustum_volume.hpp"
#include "Rendering/Effect/Constant_buffer_updater.hpp"
#include "Math/Matrix.hpp"

class Resource_manager;

namespace scene
{

class Scene;
class Light;
class Irradiance_volume;
class Light_probe;

}

namespace rendering
{

class Constant_buffer_cache;
class Directional_shadow_renderer;
class Spot_shadow_renderer;

class Deferred_lighting_renderer : public Rendering_object
{

public:

	Deferred_lighting_renderer(Rendering_tool& rendering_tool, Directional_shadow_renderer& directional_shadow_renderer, Spot_shadow_renderer& spot_shadow_renderer);

	bool init(Resource_manager& resource_manager, Constant_buffer_cache& constant_buffer_cache);

	bool resize_targets(const uint2& size, const Handle<Depth_stencil_shader_resource_view>& depth_stencil);

	void set_deferred_textures(const Handle<Shader_resource_view>& depth,
							   const Handle<Shader_resource_view>& normal,
							   const Handle<Shader_resource_view>& color,
							   const Handle<Shader_resource_view>& surface);

	void render(const scene::Scene& scene, const Rendering_context& context);

private:

	void render_emissive_light();

	void render_irradiance_volumes(const scene::Scene& scene, const Rendering_context& context);
	void render_irradiance_volume(const scene::Irradiance_volume& volume, const Rendering_context& context);

	void render_light_probes(const scene::Scene& scene, const Rendering_context& context);
	void render_surrounding_light_probe(const scene::Light_probe& light_probe, const Rendering_context& context);
	void render_light_probe(const scene::Light_probe& light_probe, const Rendering_context& context);

	void render_directional_light(const scene::Light& light, const scene::Scene& scene, const Rendering_context& context);
	void render_point_light(const scene::Light& light, const scene::Scene& scene, const Rendering_context& context);
	void render_spot_light(const scene::Light& light, const scene::Scene& scene, const Rendering_context& context);

	bool create_render_states();

	Directional_shadow_renderer& directional_shadow_renderer_;
	Spot_shadow_renderer& spot_shadow_renderer_;

	Handle<Shader_resource_view> deferred_textures_[4];

	Handle<Effect> effect_;

	Handle<Input_layout> input_layout_;
	Handle<Input_layout> volume_input_layout_;

	Box_volume     box_volume_;
	Sphere_volume  sphere_volume_;
	Frustum_volume frustum_volume_;

	Handle<Rasterizer_state> rasterizer_state_cull_back_;
	Handle<Rasterizer_state> rasterizer_state_cull_front_;

	struct Cull_state
	{
		enum Value
		{
			Back = 0,
			Front = 1
		};
	};

	Handle<Rasterizer_state> lighting_rasterizer_states_[2];

	Handle<Depth_stencil_state> lighting_ds_state_;

	Handle<Depth_stencil_state> outside_lighting_volume_ds_state_prepare_;
	Handle<Depth_stencil_state> outside_lighting_volume_ds_state_light_;

	Handle<Depth_stencil_state> inside_lighting_volume_ds_state_light_;

	Handle<Blend_state> lighting_blend_state_;
	Handle<Blend_state> z_only_blend_state_;

	struct
	{
		Effect_technique* emissive_light;
		Effect_technique* irradiance_volume;
		Effect_technique* light_probe_specular;
		Effect_technique* volume_light_probe_specular;
		Effect_technique* directional_light;
		Effect_technique* directional_light_with_shadow;
		Effect_technique* point_light;
		Effect_technique* spot_light;
		Effect_technique* spot_light_with_shadow;
	}
	techniques_;

	struct Filter_kernel
	{
		float4 filter_kernel[4];
	};

	Constant_buffer_updater<Filter_kernel> filter_kernel_;

	struct Change_per_light
	{
		float4x4 world;
		float4x4 light_transformation;
		float4   energy_and_range;
		float3   position_vs; float padding0;
		float3   direction_vs; float padding1;
		float2   shadow_linear_depth_projection;
	};

	Constant_buffer_updater<Change_per_light> change_per_light_;

	uint32_t light_2D_texture_offset0_;
	uint32_t light_2D_texture_offset1_;
	uint32_t light_probe_texture_offset_;
	uint32_t irradiance_volume_texture_offset_;
};

}


