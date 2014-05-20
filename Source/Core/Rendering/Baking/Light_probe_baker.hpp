#pragma once

#include "Light_baker_base.hpp"
#include "Resources/Handle.hpp"
#include "Math/Vector.hpp"
#include "Rendering/Input_layout.hpp"
#include "Rendering/Effect/Effect.hpp"
#include "Rendering/Effect/Constant_buffer_updater.hpp"

namespace scene
{

class Scene;

}

namespace rendering
{

class Rendering_tool;
class Rendering_context;
class Framebuffer;
class Render_target_shader_resource_view;
class Shader_resource_view;
class Generic_texture_data_adapter;
class Rasterizer_state;
class Depth_stencil_state;
class Blend_state;

namespace baking
{

class Environment_map_renderer;

class Light_probe_baker : protected Light_baker_base
{

public:

	Light_probe_baker(Rendering_tool& rendering_tool);
	~Light_probe_baker();

	bool init(Resource_manager& resource_manager, const uint2& dimensions);

	bool load_cached_data(scene::Scene& scene, Resource_manager& resource_manager) const;

	bool allocate_targets();
	void free_targets();

	void bake(scene::Scene& scene, Environment_map_renderer& environment_map_renderer, Resource_manager& resource_manager, uint32_t pass);

private:

	Handle<Shader_resource_view> filter_environment_map(const Handle<Shader_resource_view>& environment_map, bool cache, const std::string& cache_name);

	bool create_render_states();

	Handle<Shader_resource_view> create_integrated_brdf(uint32_t num_samples, Resource_manager& resource_manager) const;

	static void copy_region(unsigned char* source, const uint2& source_dimensions, uint32_t bytes_per_pixel, unsigned char* destination, const uint2& destination_dimensions);

	Rendering_tool& rendering_tool_;

	uint2 dimensions_;

	Handle<Framebuffer> framebuffer_;

	Handle<Effect> effect_;

	Handle<Render_target_shader_resource_view> filter_target_;

	struct Change_per_view
	{
		float3 ray0; float padding0;
		float3 ray1; float padding1;
		float3 ray2; float padding2;
	};

	Constant_buffer_updater<Change_per_view> change_per_view_;

	struct Change_per_roughness
	{
		float roughness;
	};

	Constant_buffer_updater<Change_per_roughness> change_per_roughness_;

	Handle<Input_layout> input_layout_;

	Handle<Rasterizer_state> rasterizer_state_;
	Handle<Depth_stencil_state> ds_state_;
	Handle<Blend_state> blend_state_;

	Generic_texture_data_adapter* probe_texture_adapter_;

	uint32_t num_roughness_levels_;
	float* roughness_;

	static const float3 s_dirs[6];
	static const float3 s_ups[6];
};

}

}


