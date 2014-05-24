#pragma once

#include "Rendering/Scene_rendering/Scene_renderer.hpp"
#include "Rendering/Viewport.hpp"

namespace scene
{

class Scene;
class Light;

}

namespace rendering
{

class Surface_collector;
class Rendering_context;

class Shadow_renderer : public Scene_renderer
{

public:

	Shadow_renderer(Rendering_tool& rendering_tool, Surface_collector& surface_collector, const uint2& dimensions);

	virtual Handle<Shader_resource_view>& shadow_map() = 0;

protected:

	Surface_collector& surface_collector_;

	uint2 shadow_map_dimensions_;

	Viewport viewport_;
};

}
