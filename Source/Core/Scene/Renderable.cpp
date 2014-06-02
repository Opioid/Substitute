#include "Renderable.hpp"
#include "Scene/Material.hpp"

namespace scene
{

Renderable::Renderable() : num_surfaces_(0), surfaces_(nullptr)
{}

Renderable::~Renderable()
{
	delete [] surfaces_;
}

void Renderable::clear()
{
    unified_surface_.clear();

	for (uint32_t i = 0; i < num_surfaces_; ++i)
    {
        surfaces_[i].clear();
    }
}

const Material* Renderable::get_material(uint32_t id) const
{
	return surfaces_[id].material;
}

const Surface& Renderable::get_unified_surface() const
{
	return unified_surface_;
}

uint32_t Renderable::get_num_surfaces() const
{
	return num_surfaces_;
}

const Surface* Renderable::get_surfaces() const
{
	return surfaces_;
}

bool Renderable::has_mixed_render_states() const
{
	return properties_.is_set(Property::Mixed_render_states);
}

void Renderable::set_mixed_render_states(bool value)
{
	properties_.set(Property::Mixed_render_states, value);
}

}
