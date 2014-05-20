#include "Main_scene_renderer.hpp"

namespace rendering
{

Main_scene_renderer::Main_scene_renderer(Rendering_tool& rendering_tool, Surface_collector& surface_collector, Surrounding_renderer& surrounding_renderer)
	: Scene_renderer(rendering_tool), surface_collector_(surface_collector), surrounding_renderer_(surrounding_renderer)
{}

Main_scene_renderer::~Main_scene_renderer()
{}

}
