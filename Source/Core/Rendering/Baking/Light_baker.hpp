#pragma once

#include "Environment_map_renderer.hpp"
#include "Irradiance_volume_baker.hpp"
#include "Light_probe_baker.hpp"

namespace rendering
{

namespace baking
{

class Light_baker
{

public:

	Light_baker(Rendering_tool& rendering_tool, Main_scene_renderer& scene_renderer);

	bool init(Resource_manager& resource_manager, const uint2& light_probe_dimensions);

	void bake(scene::Scene& scene, Resource_manager& resource_manager);

private:

	Environment_map_renderer environment_map_renderer_;

	Irradiance_volume_baker irradiance_volume_baker_;

	Light_probe_baker light_probe_baker_;

	uint2 light_probe_dimensions_;
};

}

}
