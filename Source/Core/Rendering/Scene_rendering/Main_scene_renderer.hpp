#pragma once

#include "Scene_renderer.hpp"

namespace scene
{

class Scene;

}

namespace rendering
{

class Surface_collector;
class Surrounding_renderer;
class Rendering_context;

class Main_scene_renderer : public Scene_renderer
{

public:

	Main_scene_renderer(Rendering_tool& rendering_tool, Surface_collector& surface_collector, Surrounding_renderer& surrounding_renderer);
	virtual ~Main_scene_renderer();

	virtual void render(const scene::Scene& scene, const Rendering_context& context) = 0;

protected:

	Surface_collector& surface_collector_;

	Surrounding_renderer& surrounding_renderer_;
};

}


