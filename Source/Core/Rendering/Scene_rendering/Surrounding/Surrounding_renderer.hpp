#pragma once

#include "Rendering/Rendering_object.hpp"
#include "Rendering/Color.hpp"

class Resource_manager;

namespace scene
{

class Scene;

}

namespace rendering
{

class Rendering_context;

class Surrounding_renderer : public Rendering_object
{

public:

	Surrounding_renderer(Rendering_tool& rendering_tool);
	virtual ~Surrounding_renderer();

	virtual bool init(Resource_manager& resource_manager);

	virtual void render(const scene::Scene& scene, const Rendering_context& context) = 0;

	virtual void clear(const Color3& color, const Rendering_context& context) = 0;
};

}



