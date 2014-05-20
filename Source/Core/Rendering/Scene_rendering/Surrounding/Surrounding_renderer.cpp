#include "Surrounding_renderer.hpp"

namespace rendering
{

Surrounding_renderer::Surrounding_renderer(Rendering_tool& rendering_tool) : Rendering_object(rendering_tool)
{}

Surrounding_renderer::~Surrounding_renderer()
{}

bool Surrounding_renderer::init(Resource_manager& /*resource_manager*/)
{
	return true;
}

}
