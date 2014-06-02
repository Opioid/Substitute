#include "Light_baker.hpp"
#include "Rendering/Resource_view.hpp"
#include "Rendering/Framebuffer.hpp"
#include "Rendering/Scene_rendering/Main_scene_renderer.hpp"
#include "Resources/Resource_manager.hpp"
#include "Scene/Scene.hpp"
#include "Math/Vector.inl"
#include "Timing/Stopwatch.hpp"
#include "String/String.hpp"
#include "File/File.hpp"
#include "Logging/Logging.hpp"

namespace rendering
{

namespace baking
{

Light_baker::Light_baker(Rendering_tool& rendering_tool, Main_scene_renderer& scene_renderer) :
	environment_map_renderer_(rendering_tool, scene_renderer),
	irradiance_volume_baker_(rendering_tool, uint2(64, 64)),
	light_probe_baker_(rendering_tool)
{}

bool Light_baker::init(Resource_manager& resource_manager, const uint2& light_probe_dimensions)
{
	light_probe_dimensions_ = light_probe_dimensions;

	return light_probe_baker_.init(resource_manager, light_probe_dimensions);
}

void Light_baker::bake(scene::Scene& scene, Resource_manager& resource_manager)
{
	bool cached_light_probes       = light_probe_baker_.load_cached_data(scene, resource_manager);
	bool cached_irradiance_volumes = irradiance_volume_baker_.load_cached_data(scene, resource_manager);

	if (cached_light_probes && cached_irradiance_volumes)
	{
		logging::post("Using cached indirect lighting.");
		return;
	}

	timing::Stopwatch stopwatch;
	stopwatch.start();

	Main_scene_renderer& scene_renderer = environment_map_renderer_.scene_renderer();

	uint2 previous_dimensions = scene_renderer.target_dimensions();
	Handle<Depth_stencil_shader_resource_view> previous_depth_stencil = scene_renderer.depth_stencil();

	environment_map_renderer_.allocate_targets(light_probe_dimensions_);
	light_probe_baker_.allocate_targets();

	irradiance_volume_baker_.bake(scene, environment_map_renderer_, resource_manager, 0);
	light_probe_baker_.bake(scene, environment_map_renderer_, resource_manager, 0);

	irradiance_volume_baker_.bake(scene, environment_map_renderer_, resource_manager, 1);
	light_probe_baker_.bake(scene, environment_map_renderer_, resource_manager, 1);

	light_probe_baker_.free_targets();
	environment_map_renderer_.free_targets();

	scene_renderer.resize_targets(previous_dimensions, previous_depth_stencil);

	logging::post("Baked indirect lighting (" + string::to_string_short(float(stopwatch.stop())) + " s).");
}

}

}
