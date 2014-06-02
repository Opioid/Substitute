#pragma once

#include "Rendering/Rendering_object.hpp"
#include "Rendering/Effect/Constant_buffer_updater.hpp"

class Resource_manager;

namespace scene
{

class Particle_scene;

}

namespace rendering
{

class Constant_buffer_cache;

class Particle_renderer : public Rendering_object
{

public:

	Particle_renderer(Rendering_tool& rendering_tool);

	bool init(Resource_manager& resource_manager, Constant_buffer_cache& constant_buffer_cache);

	void render(const scene::Particle_scene& scene, const Rendering_context& context);

private:

	Handle<Effect> effect_;

	Handle<Input_layout> input_layout_;

	struct
	{
		Effect_technique* particle;
	}
	techniques_;
};

}
