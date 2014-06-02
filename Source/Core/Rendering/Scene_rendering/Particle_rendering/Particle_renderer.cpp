#include "Particle_renderer.hpp"
#include "Resources/Resource_manager.hpp"

namespace rendering
{

Particle_renderer::Particle_renderer(Rendering_tool& rendering_tool) : Rendering_object(rendering_tool)
{}

bool Particle_renderer::init(Resource_manager& resource_manager, Constant_buffer_cache& constant_buffer_cache)
{
	effect_ = resource_manager.load<Effect>("Effects/Particle_rendering/Particle_renderer.effect");
	if (!effect_)
	{
		return false;
	}

	return true;
}

void Particle_renderer::render(const scene::Particle_scene& scene, const Rendering_context& context)
{

}

}
