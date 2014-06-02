#include "Particle_renderer.hpp"
#include "Rendering/Rendering_tool.hpp"
#include "Rendering/Vertex_format.hpp"
#include "Rendering/Input_layout.hpp"
#include "Resources/Resource_manager.hpp"
#include "Scene/Particles/Particle_system.hpp"

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

	techniques_.particle = effect_->technique("Particle");

//	input_layout_ = rendering_tool_.vertex_layout_cache().input_layout(*scene::Particle_system::Vertex::vertex_layout_description(), techniques_.particle->program()->signature());
//	if (!input_layout_)
//	{
//		return false;
//	}

	return true;
}

void Particle_renderer::render(const scene::Particle_scene& scene, const Rendering_context& context)
{

}

}
