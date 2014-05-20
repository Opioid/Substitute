#pragma once

#include "Light_baker_base.hpp"
#include "Ambient_cube_integrator.hpp"
#include "Resources/Handle.hpp"
#include "Math/Vector.hpp"
#include "Rendering/Rendering_context.hpp"
#include "Rendering/Input_layout.hpp"
#include "Rendering/Effect/Effect.hpp"
#include "Rendering/Effect/Constant_buffer_updater.hpp"

namespace scene
{

class Scene;
class Irradiance_volume;

}

namespace rendering
{

class Rendering_tool;
class Shader_resource_view;
class Render_target_shader_resource_view;
class Generic_texture_data_adapter;
class Rasterizer_state;
class Depth_stencil_state;
class Blend_state;

namespace baking
{

class Environment_map_renderer;

class Irradiance_volume_baker : protected Light_baker_base
{

public:

	Irradiance_volume_baker(Rendering_tool& rendering_tool, const uint2& face_dimensions);
	~Irradiance_volume_baker();

	bool load_cached_data(scene::Scene& scene, Resource_manager& resource_manager);

	void bake(scene::Scene& scene, Environment_map_renderer& environment_map_renderer, Resource_manager& resource_manager, uint32_t pass);

private:

	void bake(scene::Irradiance_volume& volume, const scene::Scene& scene, Environment_map_renderer& environment_map_renderer, const Rendering_context::Rendering_options& options,
			  bool cache, const std::string& cache_template);

	void create_volume_textures(scene::Irradiance_volume& volume, const std::vector<Ambient_cube>& ambient_cubes, bool cache, const std::string& cache_template);

	Rendering_tool& rendering_tool_;

	Ambient_cube_integrator ambient_cube_integrator_;

	uint2 face_dimensions_;
	uint2 target_dimensions_;

	uint32_t batch_size_;
};

}

}


