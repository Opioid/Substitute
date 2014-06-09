#pragma once

#include "Rendering/Rendering_object.hpp"
#include "Particle_collector.hpp"
#include "Rendering/Effect/Constant_buffer_updater.hpp"
#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"

class Resource_manager;

namespace scene
{

class Scene;
class Camera;
class Particle_system;
class Material;

}

namespace rendering
{

class Constant_buffer_cache;

class Particle_renderer : public Rendering_object
{

public:

	Particle_renderer(Rendering_tool& rendering_tool);

	bool init(Resource_manager& resource_manager, Constant_buffer_cache& constant_buffer_cache);

	void set_depth_texture(const Handle<Shader_resource_view>& depth);

	void render(const scene::Scene& scene, const Rendering_context& context);

private:

	void prepare_lighting(const scene::Scene& scene, const scene::Camera& camera);

	void prepare_material(const scene::Material* material);

	bool create_buffers();
	bool create_render_states();

	Handle<Shader_resource_view> depth_texture_;

	Handle<Effect> effect_;

	Handle<Input_layout> input_layout_;

	const scene::Material* previous_material_;
	const Effect_technique* previous_technique_;
	uint32_t previous_blend_state_;

	struct
	{
		Effect_technique* color_map;
		Effect_technique* color_map_soft;

		Effect_technique* array_color_map;
		Effect_technique* array_color_map_soft;
	}
	techniques_;

	struct
	{
		Effect_technique* color_map_soft;
	}
	lighting_techniques_;

	uint32_t num_vertices_;

	Handle<Vertex_buffer> vertex_buffer_;

	uint32_t stride_;

	struct Change_per_frame
	{
		float2 time;
	};

	Constant_buffer_updater<Change_per_frame> change_per_frame_;

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

	Handle<Rasterizer_state> rasterizer_state_;

	Handle<Depth_stencil_state> ds_state_;

	Handle<Blend_state> blend_states_[3];

	Particle_collector particle_collector_;

	uint32_t color_texture_offset_;
	uint32_t color_texture_array_offset_;
	uint32_t irradiance_volume_texture_offset_;
};

}
